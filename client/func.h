#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <shadow.h>
#include <error.h>
#include <crypt.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <syslog.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#define args_check(a,b) {if(a!=b) {printf("error args\n");\
	return -1;}}
#define check_error(ret_val,ret,func_name) {if(ret_val==ret){\
	perror(func_name);return -1;}}
#define check_thread_error(ret,func_name) {if(0!=ret){\
	printf("%s is failed %d\n",func_name,ret);return -1;}}
int send_n(int,char*,int);
int recv_n(int,char*,int);
#define FILENAME "file"
typedef struct{
	int model;//1 不带路径；2 带路径
	int len;
	char buf[1000];
}train;
