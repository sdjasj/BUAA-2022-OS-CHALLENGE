#include "lib.h"

int flag[256];

void touch(char* path, char* prefix){
	int r;
	r = user_create(path, 0);
	if (r == -E_FILE_EXISTS) {
		fwritef(1,"%s file is alreay existed!!!\n", path);
	} else if (r < 0) {
		user_panic("error in touch!!!\n");
	} else {
		fwritef(1, "%s file is not existed, so it is created!\n", path);
	}
}

void
usage(void)
{
    fwritef(1, "usage: touch [] [file...]\n");
    exit();
}

void
umain(int argc, char **argv)
{
    int i;

    ARGBEGIN{
    default:
        usage();
    case 'd':
    case 'F':
    case 'l':
        flag[(u_char)ARGC()]++;
        break;
    }ARGEND
    
    for (i=0; i<argc; i++)
        touch(argv[i], argv[i]);
	exit();
}
