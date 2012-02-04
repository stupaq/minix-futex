#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/futex.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	int shmid;
	void *seg;
	futex_t *f;

	pid_t pid1, pid2;

	shmid = shmget(IPC_PRIVATE, sizeof(futex_t) + 3 * sizeof(int), 0666|IPC_CREAT);
	if (shmid < 0) return 1;
	if ((seg = shmat(shmid, 0, 0)) == NULL)	return 1;
	f = (futex_t*)seg;

	if (futex_init(f)) return 1;
	if (futex_lock(f)) return 1;

	if ((pid1 = fork()) < 0) return 1;
	if (pid1 == 0) {
		printf("waiting pid1\n");
		if (futex_lock(f)) return 1;
		printf("dying pid1\n");
		return 0;
	}

	printf("spawned pid1\n");

	sleep(2);

	if ((pid2 = fork()) < 0) return 1;
	if (pid2 == 0) {
		printf("waiting pid2\n");
		if (futex_lock(f)) return 1;
		printf("dying pid2\n");
		return 0;
	}

	printf("spawned pid2\n");

	sleep(2);

	printf("killing pid1\n");
	kill(pid1, SIGTERM);

	if (wait(NULL) != pid1) return 1;

	printf("unlock futex\n");
	if (futex_unlock(f)) return 1;

	if (wait(NULL) != pid2) return 1;

	printf("destroying futex\n");
	if (futex_destroy(f)) return 1;
	return 0;
}
