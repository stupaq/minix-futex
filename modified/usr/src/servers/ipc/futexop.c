#include "inc.h"

struct waiting {
	endpoint_t who; /* who is waiting */
};

struct queue {
	int used;
	int count; /* number of processes waiting */
	struct waiting *list; /* list of processes waiting */
};

PRIVATE struct queue futex_list[FUTEX_MAX_COUNT];

PRIVATE void wakeup(endpoint_t who, int error) {
	message m;

	m.m_type = error;
	sendnb(who, &m);
}

#define SIZE(a) (sizeof(a)/sizeof(a[0]))

/* futex operations */
PUBLIC int do_futexop(message *m)
{
	int i;
	struct queue *fq;
	endpoint_t who;
	spinlock_t open = SPIN_UNLOCKED;
	int error = OK;
	int block = 0;
	int unlock = 0;


	switch(m->FUTEX_OPS) {
		case FUTEX_CREAT:
			/* find free slot */
			for (i = 0; i < SIZE(futex_list); i++)
				if (!futex_list[i].used)
					break;
			if (i == SIZE(futex_list)) {
				printf("IPC: futex count limit %d already reached\n", SIZE(futex_list));
				error = -1;
				goto out;
			}
			/* reserve it */
			futex_list[i].used = 1;
			futex_list[i].count = 0;
			futex_list[i].list = NULL;
			m->FUTEX_ID = i;
			break;
		case FUTEX_RMID:
			fq = futex_list + m->FUTEX_ID;
			/* wakeup every waiter */
			for (i = 0; i < fq->count; i++) {
				wakeup(fq->list[i].who, -1); /* EIDRM */
			}
			/* free waiting list */
			free(fq->list);
			fq->list = NULL;
			fq->count = 0;
			/* free slot */
			fq->used = 0;
			break;
		case FUTEX_SIGNAL:
			fq = futex_list + m->FUTEX_ID;
			if (fq->count > 0) {
				/* get first process from waiting queue */
				who = fq->list[0].who;
				/* pop from waiting queue */
				--fq->count;
				memmove(fq->list, fq->list+1, sizeof(struct waiting) * fq->count);
				/* NOTE: we leave allocated, but unused memory here,
				 * therefore we can do unconditional free in FUTEX_RMID,
				 * value of fq->list is then either NULL (never touched)
				 * or points to allocated chunk of memory */
				/* wakeup process */
				wakeup(who, OK);
			} else {
				/* in general this might be confusing (signal on empty queue crashes),
				 * but fits in our implementation */
				printf("IPC: futex signal on empty queue\n");
				error = -1;
				goto out;
			}
			break;
		case FUTEX_WAIT:
			fq = futex_list + m->FUTEX_ID;
			/* put into sleep queue */
			fq->count++;
			/* here we don't care about heap fragmentation (much like in ipc/sem.c) */
			fq->list = realloc(fq->list, sizeof(struct waiting) * fq->count);
			if (fq->list == NULL) {
				printf("IPC: futex waiting list lost\n");
				error = -1;
				goto out;
			}
			fq->list[fq->count-1].who = who_e;
			/* this operation blocks by definition and requires unlocking */
			block++;
			unlock++;
			break;
	}

out:
	/* unlock associated spinlock */
	if (unlock) {
		if (OK != sys_datacopy(SELF_E, (vir_bytes) &open, who_e, (vir_bytes) m->FUTEX_LOCK, sizeof(spinlock_t))) {
			printf("IPC: futex lock cannot be unlocked\n");
			error = -1;
			block = 0;
		}
	}

	/* reply to unblock caller */
	if (!block) {
		m->m_type = error;
		sendnb(who_e, m);
	}

	return error;
}
