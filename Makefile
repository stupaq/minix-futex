all: backup install

install: modified install.sh
	./install.sh

backup: modified backup.sh
	./backup.sh

compile: libs gnu-libs ipc

libs:
	cd /usr/src && make includes libraries

gnu-libs:
	cd /usr/src && make gnu-includes gnu-libraries

ipc:
	cd /usr/src/servers/ipc && make && make install
	kill -KILL 104

restore: backup modified restore.sh
	./restore.sh

clean:
	rm -rf backup
