#ifndef _SYS_FUTEX_H
#define _SYS_FUTEX_H

#include <sys/types.h>

/* IPC futex routines */
#define FUTEX_CREAT		0
#define FUTEX_RMID		1
#define FUTEX_SIGNAL	2
#define FUTEX_WAIT		3

/* IPC system-wide futex number limit */
#define FUTEX_MAXNUM	2048

typedef struct
{
	int val; /* current value of mutex */
	int ipc_id; /* ipc identifier of futex */
} futex_t;

/* Futex initialize and destroy functions  */
_PROTOTYPE( int futex_init, (futex_t *futex));
_PROTOTYPE( int futex_destroy, (futex_t *futex));

/* Lock and unlock futex */
_PROTOTYPE( int futex_lock, (futex_t *futex));
_PROTOTYPE( int futex_unlock, (futex_t *futex));

#endif /* _SYS_FUTEX_H */
