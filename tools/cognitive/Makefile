# Makefile for OpenCog AGI-OS cognitive kernel test program
#
# This builds userspace test programs for the cognitive kernel

CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2

all: test_cognitive

test_cognitive: test_cognitive.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f test_cognitive

install: test_cognitive
	install -D test_cognitive $(DESTDIR)/usr/bin/test_cognitive

.PHONY: all clean install