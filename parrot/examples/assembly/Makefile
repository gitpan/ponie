# $Id: Makefile,v 1.5 2003/08/12 07:57:36 leo Exp $
# Convenient makefile for assembling files.

PERL=perl    # Usually overidden by main Makefile

.SUFFIXES: .pbc .pasm

clean:
	rm -f *.pbc mops.c mops.o mops

.pasm.pbc:
	../../parrot -o $@ $<
