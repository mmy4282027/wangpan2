#include "factory.h"
int main()
{
	factory f;//主要数据结构
	int thread_num=5;
	int capacity=100;
	factory_init(&f,thread_func,thread_num,capacity);
	factory_start(&f);
	int sfd;
	tcp_start_listen(&sfd,"192.168.5.123","2000",capacity);
	int new_fd[N];
	fd_set readfds;
	fd_set tmpfds;//始终记录要监控的描述符
	FD_ZERO(&tmpfds);
	FD_SET(sfd,&tmpfds);
	memset(new_fd,-1,sizeof(new_fd));
	pque_t pq=&f.que;
	pnode_t pnew;
	int len,model,ret,i;
	while(1)
	{
		FD_ZERO(&readfds);
		memcpy(&readfds,&tmpfds,sizeof(fd_set));
		ret=select(15,&readfds,NULL,NULL,NULL);
		if(ret>0)
		{
			if(FD_ISSET(sfd,&readfds))//有客户机连接进来
			{
				for(i=0;i<N;i++)
				{
					if(new_fd[i]==-1)
					{
						break;
					}
				}
				new_fd[i]=accept(sfd,NULL,NULL);
				FD_SET(new_fd[i],&tmpfds);
				printf("one client request\n");
			}
			for(i=0;i<N;i++)
			{
				if(new_fd[i]!=-1&&FD_ISSET(new_fd[i],&readfds))//有命令进来
				{
					pnew=(pnode_t)calloc(1,sizeof(node_t));
					//接收路径以及命令
					ret=recv_n(new_fd[i],(char*)&model,4);
					if(ret<0)//客户机断开连接
					{
//						printf("server ret=%d\n",ret);
						close(new_fd[i]);
						FD_CLR(new_fd[i],&tmpfds);
						new_fd[i]=-1;
						printf("one client leave\n");
						continue;
					}
					recv_n(new_fd[i],(char*)&len,4);
					recv_n(new_fd[i],pnew->path,100);
					recv_n(new_fd[i],pnew->order,len-100);//不要回车符
					printf("model=%d,len=%d,path=%s,order=%s\n",model,len,pnew->path,pnew->order);
					pnew->new_fd=new_fd[i];
					pthread_mutex_lock(&pq->que_mutex);
					que_insert(pq,pnew);
					pthread_mutex_unlock(&pq->que_mutex);
					pthread_cond_signal(&f.cond);//发信号给子线程
				}
			}
		}
	}
}

