#include "lib.h"

void strAppend(char *src, char *target) {
	while (*src) {
		src++;
	}
	while (*target) {
		*src = *target;
		src++;
		target++;
	}
	*src = 0;
}  

void tree(char *path, int deep) {
	//deep is orignally 0
	int r;
	int i;
    struct Stat st;
	int fd, n;
    struct File f;
    char newPath[128];
 //   writef("|");
    if ((fd = open(path, O_RDONLY)) < 0)
        user_panic("open %s: %e", path, fd);
        
    while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
		if (f.f_name[0]) {
			if (f.f_type == FTYPE_DIR){
				for (i = 0; i < deep; i++){
					fwritef(1, "|   ");
				}
				fwritef(1, "|--");
				fwritef(1, "%s\n",f.f_name);
				strAppend(newPath, path);
				strAppend(newPath, f.f_name);
				strAppend(newPath,"/");
				tree(newPath, deep + 1);
			} else {
				for (i = 0; i < deep; i++){
					fwritef(1, "|   ");
				}
				fwritef(1, "|--");
				fwritef(1, "%s\n",f.f_name);
			}
		}
	}
	close(fd);
}

void
umain(int argc, char **argv)
{
    int i;
    tree("/", 0);
	exit();
}
