#include "func.h"
//循环发送
int send_n(int sfd,char* p,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=send(sfd,p+total,len-total,0);
		if(ret==0)
		{
			return -1;
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
		if(0==ret)
		{
			return -1;
		}
		total=total+ret;
	}
	return 0;
}


