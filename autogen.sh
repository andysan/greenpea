#!/bin/sh

ACLOCAL_AMFLAGS="-I m4" autoreconf -i -Wall -s
./configure
