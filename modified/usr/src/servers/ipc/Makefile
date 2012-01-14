# Makefile for IPC server
PROG=	ipc
SRCS=	main.c utility.c shm.c sem.c futexop.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
