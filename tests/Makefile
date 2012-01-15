CC=gcc
CFLAGS=-march=i686

TARGETS=numbers basic_test balance destroy_queued

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
