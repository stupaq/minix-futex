#define _SYSTEM	1
#define _MINIX 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <lib.h>
#include <minix/endpoint.h>
#include <minix/safecopies.h>

#include <sys/ipc.h>
#include <sys/futex.h>

#define TRY(code) if ((code) == -1) return -1;

PRIVATE int get_ipc_endpt(endpoint_t *pt)
{
	return minix_rs_lookup("ipc", pt);
}

/* ipc futex calls explanation:
 * FUTEX_CREAT - creates futex and sets it's ipc_id,
 * FUTEX_RMID - destroys futex,
 * FUTEX_WAIT - atomically puts caller to waiting queue and unlocks spinlock
 * 		(must be done atomically to deal with so-called 'lost wakeup' problem),
 * FUTEX_SIGNAL - wakeup one waiting process, if none returns error */

/* generic ipc futex caller, returns 0 on success, other value on error */
PRIVATE int ipc_futex_call(futex_t* futex, int action)
{
	message m;
	endpoint_t ipc_pt;
	int r;

	if (get_ipc_endpt(&ipc_pt) != OK) {
		errno = ENOSYS;
		return -1;
	}

	m.FUTEX_ID = futex->ipc_id;
	m.FUTEX_OPS = action;
	m.FUTEX_LOCK = (int) &futex->lock;

	r = _syscall(ipc_pt, IPC_FUTEX, &m);
	if (r != OK) {
		errno = r;
		return -1;
	} else if (m.m_type != OK) {
		errno = m.m_type;
		return -1;
	}

	if (action == FUTEX_CREAT) {
		assert(m.FUTEX_ID >= 0);
		futex->ipc_id = m.FUTEX_ID;
	}

	return 0;
}

/* lock spinlock */
PUBLIC void spin_lock(spinlock_t *lock)
{
	while(SPIN_LOCKED == __sync_lock_test_and_set(lock, SPIN_LOCKED))
		;
}

/* unlock spinlock */
PUBLIC void spin_unlock(spinlock_t *lock)
{
	*lock = SPIN_UNLOCKED;
}

/* initialize futex */
PUBLIC int futex_init(futex_t *futex)
{
	futex->lock = SPIN_UNLOCKED;
	futex->val = FUTEX_RAISED;
	futex->ipc_id = -1;
	futex->count = 0;

	TRY(ipc_futex_call(futex, FUTEX_CREAT));

	assert(futex->ipc_id >= 0);
	return 0;
}

/* destroy futex */
PUBLIC int futex_destroy(futex_t *futex)
{
	TRY(ipc_futex_call(futex, FUTEX_RMID));
	return 0;
}

/* lock futex */
PUBLIC int futex_lock(futex_t *futex)
{
	/* no-competition cost: 1 full barrier + O(1) */
	spin_lock(&futex->lock);
	while(futex->val == FUTEX_CLOSED) {
		++futex->count;
		/* atomic sleep and unlock futex->lock */
		TRY(ipc_futex_call(futex, FUTEX_WAIT));
		/* critical section inheritance: futex->lock is locked */
		--futex->count;
	}
	futex->val = FUTEX_CLOSED;
	spin_unlock(&futex->lock);
	return 0;
}

/* unlock futex */
PUBLIC int futex_unlock(futex_t *futex)
{
	/* no-competition cost: 1 full barrier + O(1) */
	spin_lock(&futex->lock);
	futex->val = FUTEX_RAISED;
	if (futex->count > 0) {
		TRY(ipc_futex_call(futex, FUTEX_SIGNAL));
		/* critical section inheritance: futex->lock is locked */
	} else {
		spin_unlock(&futex->lock);
	}
	return 0;
}

