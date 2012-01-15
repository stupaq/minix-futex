#include <sys/shm.h>
#include <sys/futex.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define CYCLES 100000000

int main(int argc, char** argv)
{
	int shmid;
	void *seg;
	futex_t *f;
	volatile int *val;
	volatile int *max;
	volatile int *bal;
	int i;
	volatile int temp;
	pid_t pid;

	shmid = shmget(IPC_PRIVATE, sizeof(futex_t) + 3 * sizeof(int), 0666|IPC_CREAT);
	if (shmid < 0) return 1;
	if ((seg = shmat(shmid, 0, 0)) == NULL)	return 1;
	memset(seg, 0, sizeof(futex_t) + 3 * sizeof(int));
	f = (futex_t*)seg;
	val = (int*)seg + sizeof(futex_t);
	bal = val+1;
	max = val+2;

	if (futex_init(f)) return 1;
	if ((pid = fork()) < 0) return 1;

	for (i = 0; i < CYCLES; ++i)
	{
		if (futex_lock(f)) return 1;
		temp = *val;
		temp++;
		*val = temp;
		if (pid == 0)
			(*bal)++;
		else
			(*bal)--;
		if (abs(*bal) > *max)
			*max = abs(*bal);
		if (futex_unlock(f)) return 1;
	}

	if (pid == 0) return 0;
	
	if (wait(NULL) != pid) return 1;
	fprintf(stderr, "Value: %d balance: %d\n", *val, *max);
	temp = *val;
	
	if (futex_destroy(f)) return 1;

	if (shmdt(seg)) return 1;
	if (shmctl(shmid, IPC_RMID, 0)) return 1;
	if (temp != CYCLES * 2) return 1;
	return 0;
}

