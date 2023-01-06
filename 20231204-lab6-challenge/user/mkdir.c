#include "lib.h"
/*
void mkdir(char* path) {
	int r;
	r = user_create(path, 1);
	if (r < 0) {
		user_panic("error in mkdir %d\n", r);
	}
}



void
umain(int argc, char **argv)
{
    int f, i;
	//writef("%s\n", argv[1]);
    mkdir(argv[1]);
	exit();
}
*/

int flag[256];

void createRec(char *p) {
	//type 1 is create when not exists
	char *t = p;
	char *r;
	int fd;
	if (*p == '/') {
		t = p + 1;
	}
	while (*t) {
		while (*t && *t != '/') {
			t++;
		}
		*t = 0;
		r = user_create(p, 1);
		if (r != -E_FILE_EXISTS && r!=0) {
			user_panic("error in mkdir %d\n", r);
		}
		*t = '/';
		t++;			
	}
}

void mkdir(char* path) {
	int r;
	int cnt = 0;
	char *t;
	if (flag['p']) {
		createRec(path);
	} else {
		r = user_create(path, 1);
		if (r < 0) {
			user_panic("error in mkdir %d\n", r);
		}
	}
}

void
usage(void)
{
	fwritef(1, "usage: mkdir [-p] [file...]\n");
	exit();
}

void
umain(int argc, char **argv)
{
	int i;

	ARGBEGIN{
	default:
		usage();
	case 'p':
		flag[(u_char)ARGC()]++;
		break;
	}ARGEND

	if (argc == 0)
		fwritef(1, "please input a directory name\n");
	else {
		for (i=0; i<argc; i++)
			mkdir(argv[i]);
	}
	exit();
}
