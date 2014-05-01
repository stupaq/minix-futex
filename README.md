Minix Futex
===========

A simple fast userspace mutex implementation for MINIX 3.
General idea of futexes is explained in 'Futexes Are Tricky' by
Ulrich Drepper [http://people.redhat.com/drepper/futex.pdf] [1].

This version is compatible with 

Building and Installing
-----------------------

To update Minix sources and recompile libraries and IPC server run:

	$ make
	$ make compile

**NOTE:** this will modify minix sources under /usr/src, backup copies
will be made to backup/ directory before overwriting, run `make restore`
in order to restore original files (you may need to recompile restored
sources with `make compile`).

Testing
-------

Some dummy tests are included in package, you can run them by running:

	$ ./run.sh


Copyright (c) 2012 Mateusz Machalica

