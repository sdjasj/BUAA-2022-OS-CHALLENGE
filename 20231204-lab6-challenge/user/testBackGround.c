#include "lib.h"

void
umain(int argc, char **argv) {
	int i, j, k;
	for (i = 0; i < 1000000000; i++) {
		if (i % 100000000 == 0) {
			writef("hello, i is %d\n\n", i);
		}
	}
	exit();
}
