#define _SYSTEM	1
#define _MINIX 1

#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/rs.h>

#include <lib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/futex.h>
#include <stdlib.h>
#include <errno.h>


PRIVATE int get_ipc_endpt(endpoint_t *pt)
{
	return minix_rs_lookup("ipc", pt);
}

PRIVATE int sleep_and_unlock(futex_t* futex)
{
	message m;
	endpoint_t ipc_pt;

	if (get_ipc_endpt(&ipc_pt) != OK) {
		errno = ENOSYS;
		return -1;
	}

	/* TODO */

	return (_syscall(ipc_pt, IPC_FUTEXOP, &m));
}

/* Initialize futex. */
PUBLIC int futex_init(futex_t *futex)
{
	futex->val = 1;
	sleep_and_unlock(futex);
	/* TODO */
	return 0;
}

/* Destroy futex. */
PUBLIC int futex_destroy(futex_t *futex)
{
	futex->val = 1;
	/* TODO */
	return 0;
}

/* Lock futex. */
PUBLIC int futex_lock(futex_t *futex)
{
	while(0 == __sync_lock_test_and_set(&futex->val, 0))
		;
	/* TODO */
	return 0;
}

/* Unlock futex. */
PUBLIC int futex_unlock(futex_t *futex)
{
	futex->val = 1;
	/* TODO */
	return 0;
}

