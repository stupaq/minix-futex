#include <stdio.h>
#include <sys/futex.h>

futex_t f;

int main(int argc, char** argv)
{
	if (futex_init(&f))
		return 1;
	if (futex_lock(&f))
		return 2;
	if (futex_unlock(&f))
		return 3;
	if (futex_destroy(&f))
		return 4;
	return 0;
}
