# the tap.c submodule must be checked out to build and run tests

CFLAGS += -Wall -Wextra -Wpedantic -Werror -std=c99 -g

override CPPFLAGS += -I..

TESTS = \
		01-sanity.t \
		10-new.t \
		11-finite-new.t \
		12-peek-next.t \
		13-skip-nth.t \
		20-chain.t \
		20-grep.t \
		20-map.t \
		90-smoke.t

01-sanity.t: CFLAGS += -std=c99 -pedantic -Werror

%.t: %.c ../mIterator.c ../mIterator.h ../ext/tap.c/tap.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

check: tests
	prove .

Weverything: clean
	$(MAKE) CC=clang CFLAGS="$(CFLAGS) -Weverything -Wno-padded" check

gcov: clean 
	$(MAKE) CC=gcc CFLAGS="$(CFLAGS) -fprofile-arcs -ftest-coverage" check
	gcov $(TESTS)
	which gcovr && gcovr --root=.. --filter=../mIterator.c

gprof: clean
	$(MAKE) CC=gcc CFLAGS="$(CFLAGS) -pg" check

tests: $(TESTS)

all: tests

clean:
	$(RM) $(TESTS)
	$(RM) *.gcov *.gcda *.gcno gmon.out

.PHONY: all clean check gcov gprof tests Weverything
