#include "head.h"
//循环发送
int send_n(int sfd,char* p,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=send(sfd,p+total,len-total,0);
		if(-1==ret)
		{
			printf("client is close\n");
			return -1;//客户端断开
		}
		total=total+ret;
	}
	return 0;
}


int recv_n(int sfd,char* p,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=recv(sfd,p+total,len-total,0);
		if(ret==-1||ret==0)
		{
			printf("接收到的信息为0\n");
			return -1;
		}
		total=total+ret;
	}
	return 0;
}


