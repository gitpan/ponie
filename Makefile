



all:	final

final: lib ponie

lib:
	ln -s perl/lib lib

ponie:  perl/perl
	cp perl/perl ponie

parrot/parrot:
	cd parrot && $(MAKE) parrot

perl/perl: parrot/parrot
	cd perl && $(MAKE)

clean:
	cd parrot && $(MAKE) clean
	cd perl && $(MAKE) clean
	rm -f *~
	rm -f lib
	rm -f ponie

distclean:
	error: will kill subversion

realclean:
	cd parrot && $(MAKE) realclean
	cd perl && $(MAKE) realclean
	rm -f perl/config.sh
	rm -f perl/Policy.sh
	rm -f *~
	rm -f lib
	rm -f ponie

install:
	error: parrot is not installable, so neither is ponie (yet)

test:
	cd parrot && make -k test
	cd perl && make test

.DEFAULT:
	cd parrot && $(MAKE) $<
	cd perl && $(MAKE) $<

