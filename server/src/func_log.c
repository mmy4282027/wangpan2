#include"factory.h"
int func_log(pnode_t pnode)
{
//	printf("now enter the func_log\n");
	time_t start=time(NULL);
	char *ptime;
	ptime=ctime(&start);
	int fd=open("./log_file",O_RDWR|O_CREAT|O_APPEND);
	char user_name[100]={0};
	char log_buf[1000]={0};
	int i;
	for(i=2;i<strlen(pnode->path)&&pnode->path[i]!='/';i++)
	{
		user_name[i-2]=pnode->path[i];
	}
	sprintf(log_buf,"%-10s%-10s%s",user_name,pnode->order,ptime);
	write(fd,log_buf,strlen(log_buf));
	return 0;
}
int func_enter_log(pnode_t pnode)
{

	time_t start=time(NULL);
	char *ptime;
	ptime=ctime(&start);
	int fd=open("./log_file",O_RDWR|O_CREAT|O_APPEND);
	char user_name[100]={0};
	char log_buf[1000]={0};
	int i;
	for(i=2;i<strlen(pnode->path);i++)
	{
		user_name[i-2]=pnode->path[i];
	}
	sprintf(log_buf,"%-10s%-10s%s",user_name,"connect",ptime);
	write(fd,log_buf,strlen(log_buf));
	return 0;
}
