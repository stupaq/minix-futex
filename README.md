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

Copyright and License
---------------------

Copyright Mateusz Machalica 2012.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

