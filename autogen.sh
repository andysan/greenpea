#!/bin/sh

cat m4/* > acinclude.m4
autoreconf -i -s -Wall
./configure
