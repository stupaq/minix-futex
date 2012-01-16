all: backup install

install: modified install.sh
	./install.sh

backup: modified backup.sh
	./backup.sh

compile:
	cd /usr/src && make includes gnu-includes libraries gnu-libraries
	cd /usr/src/servers/ipc && make && make install

restore: backup modified restore.sh
	./restore.sh

clean:
	rm -rf backup
