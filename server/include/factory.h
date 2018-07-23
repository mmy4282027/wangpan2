#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#define FILENAME "file"
typedef void* (*thread_func_t)(void*p);
typedef struct{
	pthread_t *pth_id;//线程id的起始地址
	int pthread_num;//创建的线程数目
	que_t que;//队列
	thread_func_t threadfunc;//线程函数
	pthread_cond_t cond;
	short start_flag;//线程是否启动标志
}factory,*pfac;
int factory_init(pfac,thread_func_t,int,int);
int factory_start(pfac);
void* thread_func(void*);
int thread_func_ls(pnode_t);
int thread_func_pwd(pnode_t);
int thread_func_cd(pnode_t,char*);
int thread_func_remove(pnode_t,char*);
int thread_func_gets(pnode_t,char*);
int thread_func_puts(pnode_t,char*);
int thread_func_fault(pnode_t);
int thread_func_test_user_name(pnode_t,char*);
int thread_func_test_user_passwd(pnode_t,char*);
int thread_func_mkdir(pnode_t,char*);
int thread_func_gets_continue(pnode_t,char*);
int tcp_start_listen(int*,char*,char*,int);
int tran_file(int);
int send_n(int,char*,int);
int recv_n(int,char*,int);
int func_log(pnode_t);
int func_enter_log(pnode_t);
//buf的1-99号是路径
typedef struct{
	int model;//设置类型，1为不带路径；2为带路径；3为下载文件,4上传文件，5为验证用户
	int len;
	char buf[4096];
}train;
#endif
