gensalt-test: gensalt-test.o gensalt.o
	gcc -o gensalt-test gensalt-test.o gensalt.o

gensalt-test.o: gensalt-test.c gensalt.h
	gcc -Wall -c gensalt-test.c

gensalt.o: gensalt.c gensalt.h
	gcc -Wall -c gensalt.c
