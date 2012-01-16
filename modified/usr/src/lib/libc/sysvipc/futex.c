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

/* atomics */
#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))
#define atomic_inc(P) __sync_fetch_and_add((P), 1)
#define atomic_dec(P) __sync_fetch_and_add((P), -1) 

/* futex val interpretations */
#define UNLOCKED	0
#define LOCKED		1
#define WAITING		2

PRIVATE int get_ipc_endpt(endpoint_t *pt)
{
	return minix_rs_lookup("ipc", pt);
}

/* generic ipc futex caller, depending on action:
 * FUTEX_CREAT - creates futex and sets it's ipc_id
 * FUTEX_RMID - destroys futex
 * FUTEX_WAIT - atomically checks if futex has desired value and if so puts caller
 * 				into waiting queue, otherwise returns
 * FUTEX_SIGNAL - wakeup up to one waiting process
 * ---
 * returns 0 on success, other value on error */
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
	m.FUTEX_ADDR = (int) &futex->val;
	m.FUTEX_VAL = WAITING;
	m.FUTEX_OPS = action;

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

/* initialize futex */
PUBLIC int futex_init(futex_t *futex)
{
	futex->val = UNLOCKED;
	futex->ipc_id = -1;

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
	int c;
	if ((c = cmpxchg(&futex->val, UNLOCKED, LOCKED)) != UNLOCKED) {
		do {
			if (c == 2 || cmpxchg(&futex->val, LOCKED, WAITING) != UNLOCKED) {
				TRY(ipc_futex_call(futex, FUTEX_WAIT));
			}
		} while ((c = cmpxchg(&futex->val, UNLOCKED, WAITING)) != UNLOCKED);
	}
	return 0;
}

/* unlock futex */
PUBLIC int futex_unlock(futex_t *futex)
{
	if (atomic_dec(&futex->val) != LOCKED) {
		futex->val = UNLOCKED;
		TRY(ipc_futex_call(futex, FUTEX_SIGNAL));
	}
	return 0;
}
