all: backup install

install: modified install.sh
	./install.sh

backup: modified backup.sh
	./backup.sh

compile: libs ipc

libs:
	cd /usr/src && make includes gnu-includes libraries gnu-libraries

ipc:
	cd /usr/src/servers/ipc && make && make install

restore: backup modified restore.sh
	./restore.sh

clean:
	rm -rf backup
