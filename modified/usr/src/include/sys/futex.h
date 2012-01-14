#ifndef _SYS_FUTEX_H
#define _SYS_FUTEX_H

#include <sys/types.h>

typedef volatile struct
{
	int val;
	/* TODO */
} futex_t;

/* Futex initialize and destroy functions  */
_PROTOTYPE( int futex_init, (futex_t *futex));
_PROTOTYPE( int futex_destroy, (futex_t *futex));

/* Lock and unlock futex */
_PROTOTYPE( int futex_lock, (futex_t *futex));
_PROTOTYPE( int futex_unlock, (futex_t *futex));


#endif /* _SYS_FUTEX_H */
