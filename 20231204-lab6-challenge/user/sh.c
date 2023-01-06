#include "lib.h"
#include "color.h"
#include <args.h>

int debug_ = 0;

EnviormentValue localVar[100];
int shellId;

int getLoaclVar(char* name, char* val, int mode, int rwMode) {
    //mode 0 is check whether has the localVar
    //mode 1 is set the var by name a
    //mode 2 is delete var
    //mode 3 is get the name's val 
    //mode 4 is create a new var of name
    //mode 5 is delete var without permission
    int i;
    int flag = 0;
    if (mode == 0) {
    	// check whether has the localVar of id
    	// 0 means has
    	//-1 means not
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
           		flag = 1;
            	break;
        	}
    	}
        if (flag == 1) {
            return 0;
        } else {
            return -1;
        }
    } else if (mode == 1) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
        		if (localVar[i].mode == 1) {
        			writef("%s is read only\n", name);
        			return -1;
				}
        		strcpy(localVar[i].value, val);
        		localVar[i].mode = rwMode > 0 ? 1 : 2;
        		return 0;	
        	}
    	}	
	} else if (mode == 2) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
               	if (localVar[i].mode == 1) {
        			writef("%s is read only\n", name);
        			return -2;
				}
        		localVar[i].mode = 0;
        		return 0;	
        	}
    	}
	} else if (mode == 3) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
        		strcpy(val, localVar[i].value);
        		return 0;	
        	}
    	}			
	} else if (mode == 4) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode == 0) {
        		localVar[i].mode =  rwMode > 0 ? 1 : 2;
        		strcpy(localVar[i].value, val);
        		strcpy(localVar[i].name, name);
			//	writef("%s\n", localVar[i].value);
			//	writef("%s\n", localVar[i].name);
			//	writef("^^^^^^^^^^^%d^^^^^^^^^^^\n", localVar[i].mode);
        		return 0;
        	}
    	}
	} else if (mode == 5) {
        for (i = 0; i < 100; i++) {
        	if (localVar[i].mode != 0 && strcmp(name, localVar[i].name) == 0) {
        		localVar[i].mode = 0;
        		return 0;	
        	}
    	}
	}
	return -1;
}

void showVar() {
	int i;
	int r;
	char name[16] = {0};
	char val[16] = {0};
	for (i = 0; i < 100; i++) {
		r = syscall_getGloablVar(name, val, 6, shellId, i);
		if (r == 0) {
			//fwritef(1, "^^^^^^%d^^^^^^^^^\n", i);
			fwritef(1, "shellId: %d " "name: " BLUE(%s) "  value:%s" " \n",shellId, name, val);
		}
	}
	//writef("global successful\n");
	for (i = 0; i < 100; i++) {
		if (localVar[i].mode != 0) {
			//writef("11111\n");
			fwritef(1, "shellId: %d " " name: " GREEN(%s) "  value:%s" " \n",shellId,  localVar[i].name, localVar[i].value);
		}
	}
}


void declare(char* nameAndVal, int mode) {
	u_int x = mode & 0x1;
	u_int READONLY = mode & 0x2;
	char name[MAXENVLEN] = {0};
	char val[MAXENVLEN] = {0};
	int i, j;
	int r;
	
	
	if (mode == -1) {
		showVar();
		return;
	}
	
	
	for (i = 0; nameAndVal[i] != '=' && nameAndVal[i] != 0; i++){
		name[i] = nameAndVal[i];
	}
	if (nameAndVal[i]) {
		for (j = 0, i++; nameAndVal[i]; i++, j++){
			val[j] = nameAndVal[i];
		}
	}
//	writef("%s\n%s\n",name, val);
	
	if (x) {
		r = getLoaclVar(name, val, 0, 0);
		if (r == 0) { //local exists
			r = syscall_getGloablVar(name, val, 0, shellId, 0);
			if (r == 0) { //has global var
			//	writef("00000000000\n");
				r = syscall_getGloablVar(name, val, 1, shellId, READONLY);
				if (r) return;
				r = getLoaclVar(name, val, 5, 0);
				if (r) return;
			} else { //no global var
			//	writef("1111111111111\n");
				r = syscall_getGloablVar(name, val, 4, shellId, READONLY);
				if (r) return;
				r = getLoaclVar(name, val, 5, 0);
				if (r) return;
			}
		} else {
			r = syscall_getGloablVar(name, val, 0, shellId, 0);
			if (r == 0) { //has global var
				r = syscall_getGloablVar(name, val, 1, shellId, READONLY);
				if (r) return;
				r = getLoaclVar(name, val, 5, 0);
			} else { //no global var
				//writef("no global var\n");
				r = syscall_getGloablVar(name, val, 4, shellId, READONLY);
				if (r) return;
			}
		}
	} else {
		r = syscall_getGloablVar(name, val, 0, shellId, 0);
		if (r == 0) { //global exists
			r = syscall_getGloablVar(name, val, 1, shellId, READONLY);
			if (r) return;
		} else {
		//	writef("local\n");
			r = getLoaclVar(name, val, 0, READONLY);
			if (r == 0) { //global not exists but local exist
				r = getLoaclVar(name, val, 1, READONLY);
				if (r) return;
			} else { //global and local both not exists
				r = getLoaclVar(name, val, 4, READONLY);
				if (r) return;
			}
		}
	}
	
}

void
declareMain(int argc, char **argv) {
    int i;
	int flag[256] = {0};
	int mode = 0;

    ARGBEGIN
    {
        default:
            usage();
        case 'x':
        case 'r':
            flag[(u_char) ARGC()]++;
        break;
    }
    ARGEND
    //00 none 01 -x 10 -r 11 -rx
    if (flag['x']) {
    	mode += 1;
	}
	if (flag['r']) {
		mode += 2;
	}
	if (argc == 0) {
		declare(0, -1);
		return;
	}
    for (i = 0; i < argc; i++){
	//	writef("^^^^^^^%s\n", argv[i]);
    	declare(argv[i], mode);
	}
}

void unset(char* name) {
	int r;
	int i;
	r = syscall_getGloablVar(name, 0, 2, shellId, 0);
	if (r == 0) {
		return;
	} else if (r == -2) {
		fwritef(1, " unset: %s: cannot unset: readonly variable", name);
		return;
	}
	r = getLoaclVar(name, 0, 2, 0);
	if (r == -2) {
		fwritef(1, " unset: %s: cannot unset: readonly variable", name);
		return;
	} else if (r == 0) {
		return;
	}
}




void
unsetMain(int argc, char **argv) {
    int i;
	int flag[256] = {0};
	
    ARGBEGIN
    {
        default:
            usage();
        case 'v':
            flag[(u_char) ARGC()]++;
        break;
    }
    ARGEND
    
    if (argc == 0) {
    	fwritef(1, "unset need a name of var\n");
    	return;
	}
    for (i = 0; i < argc; i++){
    	unset(argv[i]);
	}
}


void getShellId() {
    int fd, n;
    int r;
    int i;
    char tmp[12] = {0};
	if ((fd = open("/shellId", O_RDWR)) < 0)
    	user_panic("open %s: error in getShellId\n", fd);
    if ((r = seek(fd, 0)) < 0) {
        user_panic("error in seek getShellId\n");
    }
    n = read(fd, tmp, (long)sizeof tmp);
    int ans = 0;
    for (i = 0; tmp[i]; i++) {
    	ans = ans * 10 + tmp[i] - '0';
	}
	close(fd);
	shellId = ans;
}

void addShellIdToFile(int type) {
	int fd, n;
    int r;
    int i, j;
    int ans = shellId + type;
    char tmp[12] = {0};
    i = 0;
	ans = ans < 0 ? 0 : ans;
	//writef("ans:    %d\n", ans);
    while (ans) {
    	tmp[i++] = ans % 10 + '0';
    	ans /= 10;
	}
	for (j = i - 1, i = 0; i < j; i++, j--){
		char c = tmp[i];
		tmp[i] = tmp[j];
		tmp[j] = c;
	}
	//writef("tmp:   %s\n", tmp);
	if ((fd = open("/shellId", O_RDWR)) < 0)
    	user_panic("open %s: error in addShellIdToFile\n", fd);
    if ((r = seek(fd, 0)) < 0) {
        user_panic("error in seek addShellIdToFile\n");
    }
    r = write(fd, tmp, strlen(tmp));
	if (r < 0) {
		user_panic("error in writeback in addShellIdToFile\n");
	} 
	close(fd);
}

void saveCommandToHistory(char *buf) {
	int fd, n;
	int r;
    if ((fd = open("/.history", O_RDWR | O_APPEND)) < 0)
        user_panic("open %d error in saveCommandToHistory\n", fd);
    r = write(fd, buf, strlen(buf));
    if (r < 0) {
    	user_panic("error in saveCommandToHistory of write Buf\n");
	}
	r = write(fd, "\n", 1);
	if (r < 0) {
    	user_panic("error in saveCommandToHistory of write enter\n");
	}
	close(fd);
}

void getHistoryCommand(char* command, int clear, int upOrDown, int* repeat){
	static int curLine = 0;
	int fd, n;
	int r;
	int i;
	char buf[8192];
	char cmp[256];
	int j;
	//upOrDown 1 is up, 0 is down
	if (clear) {
		curLine = 0;
		return;
	}
	if ((fd = open("/.history", O_RDONLY)) < 0)
    	user_panic("open %s: error  in histroy\n", fd);
    if ((r = seek(fd, 0)) < 0) {
        user_panic("error in seek history\n");
    }
    n = read(fd, buf, (long)sizeof buf);
	if (upOrDown == 1) {
		curLine++;
	} else if (upOrDown == 0){
		curLine--;
	} else if (upOrDown == -1) {
	    for (i = n - 2; i > 0; i--) {
    	    if (buf[i] == '\n') {
        	    i++;
            	break;
        	}
   		}
   		for (j = 0; i < n; i++, j++){
        	cmp[j] = buf[i];
        	if (buf[i] == '\n') {
            	cmp[j] = 0;
            	break;
        	}
    	}
    	if (strcmp(cmp, command) == 0) {
        	*repeat = 1;
    	} else {
        	*repeat = 0;
    	}
		close(fd);
		return;
	} 
	int fast = -1;
    for (i = n - 1; i >= 0; i--) {
	//	writef("%c", buf[i]);
        if (*(buf + i) == '\n') {
            fast++;
            if (fast >= curLine) {
            	i++;
            	break;
			}
        }
		if (i == 0) {
			fast++;
			break;
		}
    }
	//writef("%d\n", i);
    char* tmp = command;
    for (; i < n; i++, tmp++){
    	*tmp = buf[i];
    	if (buf[i] == '\n') {
			*tmp = 0;
    		break;
		}
	}
	for (i = n - 2; i > 0; i--) {
		if (buf[i] == '\n') {
			i++;
			break;
		}
	}
	for (j = 0; i < n; i++, j++){
		cmp[j] = buf[i];
		if (buf[i] == '\n') {
			cmp[j] = 0;
			break;
		}
	}
	if (strcmp(cmp, command) == 0) {
		*repeat = 1;
	} else {
		*repeat = 0;
	}
    if (fast <= curLine) {
        curLine = fast;
    }
    if (curLine <= 0) {
        curLine = 0;
        command[0] = 0;
    }
	//writef("%s",command);
	close(fd);
}



//
// get the next token from string s
// set *p1 to the beginning of the token and
// *p2 just past the token.
// return:
//	0 for end-of-string
//	> for >
//	| for |
//	w for a word
//
// eventually (once we parse the space where the nul will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

void changeForVar(char *src, char *target) {
	int flag = 0;
	int i;
	int r;
	char tmp[16] = {0};
	char val[16] = {0};
	char* s = src;
	char* t = target;
	while (*s) {
	//	writef("%c\n", *s);
		if ((*s) == '$') {
			s++;
			break;
		}
		*t = *s;
		t++;
		s++;
	}
//	writef("adawd\n");
//	writef("%c\n", *s);
	if (*s) {
		for (i = 0; *s && !strchr(WHITESPACE, *s); s++, i++) {
			tmp[i] = *s;
		//	writef("%c\n", *s);
		}
		r = syscall_getGloablVar(tmp, val, 3, shellId, 0);
		if (r == 0) { //success of global
			for (i = 0; val[i]; i++, t++) {
				*t = val[i];
			}
			*t = 0;
			return;
		} else { // no global
		//	writef("adawd\n");
			r = getLoaclVar(tmp, val, 3, 0); 
			if (r == 0) { // has local
				for (i = 0; val[i]; i++, t++) {
					*t = val[i];
				}
				*t = 0;
				return;
			} else { // no global and local
				fwritef(1, "$ is not follow a var\n");
				*target = 0;
				return;
			}
		}
	} else { //no $ or just a $
		*t = 0;
		return;
	}
	
}

int
_gettoken(char *s, char **p1, char **p2)
{
	int t;

	if (s == 0) {
		//if (debug_ > 1) writef("GETTOKEN NULL\n");
		return 0;
	}

	//if (debug_ > 1) writef("GETTOKEN: %s\n", s);

	*p1 = 0;
	*p2 = 0;

	while(strchr(WHITESPACE, *s))
		*s++ = 0;
	if(*s == 0) {
	//	if (debug_ > 1) writef("EOL\n");
		return 0;
	}
	if (strchr("\"", *s)){
		*p1 = ++s;
   		while(*s != '\"')
       		 s++;
		*s = 0;
    	*p2 = ++s;
		return 'w';
	}
	if(strchr(SYMBOLS, *s)){
		t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
//		if (debug_ > 1) writef("TOK %c\n", t);
		return t;
	}
	*p1 = s;
	while(*s && !strchr(WHITESPACE SYMBOLS, *s))
		s++;
	*p2 = s;
	if (debug_ > 1) {
		t = **p2;
		**p2 = 0;
//		writef("WORD: %s\n", *p1);
		**p2 = t;
	}
	return 'w';
}



int
gettoken(char *s, char **p1)
{
	static int c, nc;
	static char *np1, *np2;
	if (s) {
		nc = _gettoken(s, &np1, &np2); //np1---begin np2---end 
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 16
void
runcmd(char *s)
{
	char *argv[MAXARGS], *t;
	int argc, c, i, r, p[2], fd, rightpipe;
	int fdnum;
	int pend = 0;
	int pid1;
	int j;
	int pid;
	rightpipe = 0;
	gettoken(s, 0); //store the first word
again:
	argc = 0;
	for(;;){
		c = gettoken(0, &t);
	//	writef("%c",c);
		switch(c){
		case 0:
			goto runit;
		case '&':
			pend = 1; //Suspend to background
			break;
		case ';':
			pid1 = fork();
			if (pid1 == 0) {
				goto runit;
			} else {
				wait(pid1);
				pend = 0;
				rightpipe = 0;
				goto again;
			}
			break;
		case 'w':
			if(argc == MAXARGS){
				writef("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
		//	writef("%s\n",t);
			break;
		case '<':
			if(gettoken(0, &t) != 'w'){
				writef("syntax error: < not followed by word\n");
				exit();
			}
			// Your code here -- open t for reading,
			// dup it onto fd 0, and then close the fd you got.
			fdnum = open(t, O_RDONLY);
			dup(fdnum, 0);
			close(fdnum);
	//		user_panic("< redirection not implemented");
			break;
		case '>':
			// Your code here -- open t for writing,
			// dup it onto fd 1, and then close the fd you got.
            if(gettoken(0, &t) != 'w'){
                writef("syntax error: > not followed by word\n");
                exit();
            }
			fdnum = open(t, O_WRONLY);
			dup(fdnum, 1);
			close(fdnum);
	//		user_panic("> redirection not implemented");
			break;
		case '|':
			// Your code here.
			// 	First, allocate a pipe.
			//	Then fork.
			//	the child runs the right side of the pipe:
			//		dup the read end of the pipe onto 0
			//		close the read end of the pipe
			//		close the write end of the pipe
			//		goto again, to parse the rest of the command line
			//	the parent runs the left side of the pipe:
			//		dup the write end of the pipe onto 1
			//		close the write end of the pipe
			//		close the read end of the pipe
			//		set "rightpipe" to the child envid
			//		goto runit, to execute this piece of the pipeline
			//			and then wait for the right side to finish
			r = pipe(p);
			rightpipe = fork();
			if (rightpipe == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				goto again;
			} else {
				dup(p[1], 1);
				close(p[0]);
				close(p[1]);
				goto runit;
			}
			//user_panic("| not implemented");
			break;
		}
	}

char target[MAXARGS][16] = {0};
runit:
	if(argc == 0) {
		if (debug_) writef("EMPTY COMMAND\n");
		return;
	}
	argv[argc] = 0;
	if (1) {
		writef("[%08x] SPAWN:", env->env_id);
		for (i=0; argv[i]; i++) {
			changeForVar(argv[i], target[i]);
			argv[i] = target[i];
			writef(" %s", argv[i]);
		}
		writef("\n");
	}
//	writef("^^^^^^^^^^^^%s^^^^^^^^^^^^^\n, %d",argv[0], strcmp(argv[0], "declare"));
	if (strcmp(argv[0], "declare") == 0) {
	//	writef("1111\n");
		declareMain(argc, argv);
	//	exit();
		goto end;
	}
	if (strcmp(argv[0], "sh") == 0) {
		addShellIdToFile(1);
	}
	if (strcmp(argv[0], "unset") == 0) {
		unsetMain(argc, argv);
		goto end;
	}
	if ((r = spawn(argv[0], argv)) < 0)
		writef("spawn %s: %e\n", argv[0], r);
end:
	close_all();
	if (r >= 0) {
		if (debug_) writef("[%08x] WAIT %s %08x\n", env->env_id, argv[0], r);
		if (pend == 0) {
			wait(r);
		} else {
			//writef("fuck\n");
			pid = fork();
			//writef("fuck\n");
			//writef("pid:   %d\n", pid);
			if (pid == 0) {
				writef("[%08x] is excuting the cmd in backGround%s %08x\n", env->env_id, argv[0], r);
				wait(r);
				writef("\n\n[%08x] has finished cmd in the backGround\n\n", env->env_id);
			}
		} 
	} 
	if (rightpipe) {
		if (debug_) writef("[%08x] WAIT right-pipe %08x\n", env->env_id, rightpipe);
		wait(rightpipe);
	}
//	writef("father is here!!!!\n");
	exit();
}


void
readline(char *buf, u_int n)
{
	int i, r;
	int repeat;
	int isHistoryCmd = 0;
	r = 0;
	for(i=0; i<n; i++){
		if((r = read(0, buf+i, 1)) != 1){
			if(r < 0)
				writef("read error: %e", r);
			exit();
		}
		if (buf[i] == '\x7f') { //<----
			if (i == 0) {
				i--;
				continue;
			}
			i-=2;
			fwritef(1,"\x1b[1D\x1b[K"); //back one
		}
		//writef("%c\n",*(buf+i));
		//writef("%c\n",*(buf+i));
		if (buf[i] == '\x1b') {
			repeat = 0;
			i++;
       		if((r = read(0, buf+i, 1)) != 1){
            	if(r < 0)
                	writef("read error: %e", r);
            	exit();
        	}
			if (buf[i] != '[') {
				writef("read error: %e", r);
                exit();
			}
			i++;
            if((r = read(0, buf+i, 1)) != 1){
                if(r < 0)
                    writef("read error: %e", r);
                exit();
            }
			if (buf[i] == 'A') {
				fwritef(1, "\x1b[1B\x1b[2K");
			//	fwritef(1, "\x1b[1B\x1b[%dD\x1b[K", i);
				getHistoryCommand(buf, 0, 1, &repeat);
				fwritef(1, "%s", buf);
			} else if (buf[i] == 'B') {
			//	fwritef(1, "\x1b[1A\x1b[2K");
				fwritef(1, "\x1b[%dD\x1b[K", i);
				getHistoryCommand(buf, 0, 0, &repeat);
				fwritef(1, "%s", buf);
			}
			i = strlen(buf) - 1;
		}
		if(buf[i] == '\b'){
			if(i > 0)
				i -= 2;
			else
				i = 0;
		}
		if(buf[i] == '\r' || buf[i] == '\n'){
			buf[i] = 0;
			getHistoryCommand(buf, 0, -1, &repeat);
			if (!repeat)
				saveCommandToHistory(buf);
			return;
		}
	}
	writef("line too long\n");
	while((r = read(0, buf, 1)) == 1 && buf[0] != '\n')
		;
	buf[0] = 0;
}	

char buf[1024];

void
usage(void)
{
	writef("usage: sh [-dix] [command-file]\n");
	exit();
}

void
umain(int argc, char **argv)
{
	int r, interactive, echocmds;
	interactive = '?';
	echocmds = 0;
	writef("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	writef("::                                                         ::\n");
	writef("::              Super Shell  V0.0.0_1                      ::\n");
	writef("::                                                         ::\n");
	writef(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
//	writef("^^^^^^^^shell envid is %d\n",env->env_id);
	ARGBEGIN{
	case 'd':
		debug_++;
		break;
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}ARGEND

	if(argc > 1)
		usage();
	if(argc == 1){
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0)
			user_panic("open %s: %e", r);
		user_assert(r==0);
	}
	if(interactive == '?')
		interactive = iscons(0);
//	user_create("/.history", 0); //has bug
	getShellId(); // getId
	r = syscall_getGloablVar(0, 0, 5, shellId, 0);
	//important!!! share mem
	r = syscall_mem_map(0, localVar, 0, localVar, PTE_V | PTE_R | PTE_FORK);
	writef("^^^^^^shell ID:  %d^^^^^^\n", shellId);
//	showVar();
	for(;;){
		if (interactive)
			fwritef(1, "\n$ ");
		getHistoryCommand(0, 1, 0, 0);
		readline(buf, sizeof buf);	
		if (strcmp(buf, "exit") == 0) {
			r = syscall_getGloablVar(0,0,7,shellId,0);
			addShellIdToFile(-1);
			exit();
		}
		if (buf[0] == '#')
			continue;
		if (echocmds)
			fwritef(1, "# %s\n", buf);
		if ((r = fork()) < 0)
			user_panic("fork: %e", r);
		if (r == 0) {
			runcmd(buf);
			exit();
			return;
		} else
			wait(r); //waite for chile shell finish
		//runcmd(buf);
	}
}

