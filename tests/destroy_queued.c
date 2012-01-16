#include <sys/shm.h>
#include <sys/futex.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int shmid;
	void *seg;
	futex_t *f;
	pid_t pid;

	if ((shmid = shmget(IPC_PRIVATE, sizeof(futex_t), 0666|IPC_CREAT)) < 0) return 1;
	if ((seg = shmat(shmid, 0, 0)) == NULL)	return 1;

	f = (futex_t*)seg;
	if (futex_init(f)) return 1;

	futex_lock(f);
	printf("parent: lock\n");

	if ((pid = fork()) < 0) return 1;

	if (pid == 0) {
		printf("child: hang\n");
		if (futex_lock(f))
			printf("child: error wakeup\n");
		printf("child: exit\n");
		return 0;
	}
	
	sleep(1);

	if (futex_destroy(f)) return 1;
	printf("parent: destroy\n");

	if (shmdt(seg)) return 1;
	if (shmctl(shmid, IPC_RMID, 0)) return 1;

	printf("parent: exit\n");
	return 0;
}

