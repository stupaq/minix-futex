#ifndef _SYS_FUTEX_H
#define _SYS_FUTEX_H

#include <sys/types.h>

typedef volatile i32_t spinlock_t;

/* spinlock state */
#define	SPIN_LOCKED		1
#define SPIN_UNLOCKED	0

/* lock and unlock spinlock*/
_PROTOTYPE( void spin_lock, (spinlock_t *lock));
_PROTOTYPE( void spin_unlock, (spinlock_t *lock));

/* futex state */
#define FUTEX_RAISED	1
#define FUTEX_CLOSED	0

/* IPC futex routines */
#define FUTEX_CREAT		0
#define FUTEX_RMID		1
#define FUTEX_SIGNAL	2
#define FUTEX_WAIT		3

/* maximum number of mutexes in use */
#define FUTEX_MAX_COUNT	2048

typedef struct
{
	spinlock_t lock; /* userspace temporary lock */
	int val; /* current value of mutex */
	int count; /* number of processes waiting on wait queue */
	int ipc_id; /* ipc identifier of futex */
} futex_t;

/* Futex initialize and destroy functions  */
_PROTOTYPE( int futex_init, (futex_t *futex));
_PROTOTYPE( int futex_destroy, (futex_t *futex));

/* Lock and unlock futex */
_PROTOTYPE( int futex_lock, (futex_t *futex));
_PROTOTYPE( int futex_unlock, (futex_t *futex));

#endif /* _SYS_FUTEX_H */
