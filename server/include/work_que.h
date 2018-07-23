#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"
typedef struct tag_node{
	int new_fd;
	char order[1000];
	char path[1000];
	struct tag_node* pNext;
}node_t,*pnode_t;

typedef struct{
	pnode_t que_head,que_tail;
	int que_capacity;
	int size;
	pthread_mutex_t que_mutex;
}que_t,*pque_t;
void que_insert(pque_t,pnode_t);
void que_get(pque_t,pnode_t*);
void que_insert_exit(pque_t,pnode_t);
#endif
