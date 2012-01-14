#include <sys/shm.h>
#include <sys/futex.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#define CYCLES 100000000

int main(int argc, char** argv)
{
	int shmid;
	void *seg;
	futex_t *f;
	volatile int *val;
	int i;
	volatile int temp;
	pid_t pid;

	shmid = shmget(IPC_PRIVATE, sizeof(futex_t) + sizeof(int), 0666|IPC_CREAT);
	if (shmid < 0) return 1;
	if ((seg = shmat(shmid, 0, 0)) == NULL)	return 1;
	f = (futex_t*)seg;
	val = (int*)seg + sizeof(futex_t);

	if (futex_init(f)) return 1;
	if ((pid = fork()) < 0) return 1;

	for (i = 0; i < CYCLES; ++i)
	{
		if (futex_lock(f)) return 1;
		temp = *val;
		temp++;
		*val = temp;
		if (futex_unlock(f)) return 1;
	}

	if (pid == 0) return 0;
	
	if (wait(NULL) != pid) return 1;
	fprintf(stderr, "Value: %d\n", *val);
	temp = *val;
	
	if (futex_destroy(f)) return 1;

	if (shmdt(seg)) return 1;
	if (shmctl(shmid, IPC_RMID, 0)) return 1;
	if (temp != CYCLES * 2) return 1;
	return 0;
}

