#include "work_que.h"

void que_insert(pque_t pq,pnode_t pnew)
{
	if(!pq->que_head)
	{
		pq->que_head=pnew;
		pq->que_tail=pnew;
	}else{
		pq->que_tail->pNext=pnew;
		pq->que_tail=pnew;
	}
	pq->size++;
}

void que_get(pque_t pq,pnode_t* pcur)
{
	*pcur=pq->que_head;
	if(NULL==*pcur)
	{
		return;
	}
	if(-1==(*pcur)->new_fd)
	{
		pthread_mutex_unlock(&pq->que_mutex);
		pthread_exit(NULL);
	}
	pq->que_head=pq->que_head->pNext;
	if(!pq->que_head)
	{
		pq->que_tail=NULL;
	}
	pq->size--;
}
//头插法
void que_insert_exit(pque_t pq,pnode_t pnew)
{
	if(NULL==pq->que_head)
	{
		pq->que_head=pnew;
		pq->que_tail=pnew;
	}else{
		pnew->pNext=pq->que_head;
		pq->que_head=pnew;
	}	
	pq->size++;
}
