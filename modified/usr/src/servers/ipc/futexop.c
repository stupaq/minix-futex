#include "inc.h"

struct waiting {
	endpoint_t who; /* who is waiting */
};

struct queue {
	int used; /* used/free entry */
	int count; /* number of processes waiting */
	struct waiting *list; /* list of processes waiting */
};

/* system-wide futex count limit */
#define	FUTEX_MAX_COUNT	2048

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
	int value = 0;
	int error = OK;
	int block = 0;

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
			}
			/* else: does nothing */
			break;
		case FUTEX_WAIT:
			if (OK != sys_datacopy(who_e, (vir_bytes) m->FUTEX_ADDR, SELF_E, (vir_bytes) &value, sizeof(int))) {
				printf("IPC: futex value cannot be read\n");
				error = -1;
			}
			if (value == m->FUTEX_VAL) {
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
				/* this operation blocks caller */
				block++;
			}
			break;
	}

out:
	/* reply to unblock caller */
	if (!block) {
		m->m_type = error;
		sendnb(who_e, m);
	}

	return error;
}
