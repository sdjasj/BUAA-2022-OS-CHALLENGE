#include "lib.h"

char flag[256];

char buf[8192];

int toNum(char *s) {
	int ans = 0;
	while(*s) {
		ans = ans * 10 + (*s) - '0';
		s++;
	}
	return ans;
}

void history(int arg) {
	int fd, n;
	int r;
	int i;
	int line;
	int fast = -1;
	int allLine = 0;
	if (flag['c']) {
    	if ((fd = open("/.history", O_RDONLY)) < 0)
        	user_panic("open %d: error in histroy\n",  fd);
        if ((r = seek(fd, 0)) < 0) {
        	user_panic("error in seek history\n");
		}
	//	seek(fd, 0);	
		r = ftruncate(fd, 0);
		if (r) user_panic("error in ftruncate in history\n");
		return;
	}
	if (arg == -1) {
    	if ((fd = open("/.history", O_RDONLY)) < 0)
        	user_panic("open %d: error in histroy\n",  fd);
        if ((r = seek(fd, 0)) < 0) {
        	user_panic("error in seek history\n");
		}
	//	seek(fd, 0);
		n=read(fd, buf, (long)sizeof buf);
		line = 1;
		fwritef(1, " %d ", line);
		for (i = 0; i < n; i++) {
			fwritef(1, "%c", *(buf + i));
			if (*(buf + i) == '\n') {
				if (buf[i + 1]) {
					fwritef(1, " %d ", ++line);
				} else {
					break;
				}
			}
		}
	} else {
		if ((fd = open("/.history", O_RDWR | O_APPEND)) < 0)
        	user_panic("open %d: error in histroy\n",  fd);
	    if ((r = seek(fd, 0)) < 0) {
	        user_panic("error in seek history\n");
	    }
    	n = read(fd, buf, (long)sizeof buf);
    	for (i = 0; i < n; i++) {
    		if (*(buf + i) == '\n') {
	            allLine++;
	        }
		}
		for (i = n - 1; i >= 0; i--) {
		//	writef("%c", buf[i]);
	        if (*(buf + i) == '\n') {
	            fast++;
	            if (fast == arg) {
	            	i++;
	            	break;
				}
	        }
			if (i == 0) {
				fast++;
				break;
			}
    	}
		line = ( allLine - arg + 1 ) > 0 ? ( allLine - arg + 1 ) : 1;
		fwritef(1, " %d ", line);
		for (; i < n; i++) {
			fwritef(1, "%c", *(buf + i));
			if (*(buf + i) == '\n') {
				if (buf[i + 1]) {
					fwritef(1, " %d ", ++line);
				} else {
					break;
				}
			}
		}
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
    case 'c':
        flag[(u_char)ARGC()]++;
        break;
    }ARGEND
    if (argc == 1) {
    	history(toNum(argv[0]));
	} else {
		history(-1);
	}
}
