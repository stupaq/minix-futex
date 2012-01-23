#include <sys/shm.h>
#include <sys/futex.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#define CYCLES 10000000
#define PROCNUM 20

#define TRY(code) if (code) {printf("ERROR\n"); return 1;}

int main(int argc, char** argv)
{
	int shmid;
	void *seg;
	futex_t *f;
	volatile int *val;
	int i, j;
	volatile int temp;
	pid_t pid;

	shmid = shmget(IPC_PRIVATE, sizeof(futex_t) + sizeof(int), 0666|IPC_CREAT);
	TRY(shmid < 0)
	TRY((seg = shmat(shmid, 0, 0)) == NULL)
	f = (futex_t*)seg;
	val = (int*)seg + sizeof(futex_t);

	TRY(futex_init(f))
	TRY(futex_lock(f))

	for (j = 0; j < PROCNUM; ++j) {
		TRY((pid = fork()) < 0)
		if (pid == 0) {
			for (i = 0; i < CYCLES; ++i)
			{
				TRY(futex_lock(f))
				temp = *val;
				temp++;
				*val = temp;
				TRY(futex_unlock(f))
			}
			return 0;
		}
	}
	TRY(futex_unlock(f))

	for (j = 0; j < PROCNUM; ++j) {
		wait(NULL);
	}
	temp = *val;
	printf("Value: %d\n", temp);

	TRY(futex_destroy(f))

	TRY(shmdt(seg))
	TRY(shmctl(shmid, IPC_RMID, 0))
	TRY(temp != CYCLES * PROCNUM)
	return 0;
}

