#include"factory.h"
void* thread_func(void* p)
{
	pfac pf=(pfac)p;
	pque_t pq=&pf->que;
	pnode_t pcur;
	char command[100]={0};
	char parameter[100]={0};
	int i,j;
	while(1)
	{
		bzero(command,sizeof(command));
		bzero(parameter,sizeof(parameter));
		pthread_mutex_lock(&pq->que_mutex);
		if(0==pq->size)
		{
			pthread_cond_wait(&pf->cond,&pq->que_mutex);
		}
		que_get(pq,&pcur);
		pthread_mutex_unlock(&pq->que_mutex);
		//		printf("线程激活\n");
		if(pcur!=NULL)
		{
			//			printf("pcur->order is %s,pcur->path is %s\n",pcur->order,pcur->path);
			for(j=0,i=0;pcur->order[i]!=' '&&i<strlen(pcur->order);i++,j++)
			{
				command[j]=pcur->order[i];
			}
			//			printf("i=%d,j=%d\n",i,j);
			//			printf("command is %s\n",command);
			i++;
			for(j=0;i<strlen(pcur->order);i++,j++)
			{
				parameter[j]=pcur->order[i];
			}
			//			printf("i=%d,j=%d\n",i,j);		
			//			printf("parameter is %s\n",parameter);
			if(strcmp(command,"ls")==0)
			{
				thread_func_ls(pcur);
			}
			else if(strcmp(command,"pwd")==0)
			{
				thread_func_pwd(pcur);
			}else if(strcmp(command,"cd")==0)
			{
				thread_func_cd(pcur,parameter);
			}
			else if(strcmp(command,"remove")==0)
			{
				thread_func_remove(pcur,parameter);
			}
			else if(strcmp(command,"gets")==0)
			{
				thread_func_gets(pcur,parameter);
			}
			else if(strcmp(command,"gets_continue")==0)
			{
				thread_func_gets_continue(pcur,parameter);
			}
			else if(strcmp(command,"puts")==0)
			{
				thread_func_puts(pcur,parameter);
			}
			else if(strcmp(command,"test_user_name")==0)
			{
				thread_func_test_user_name(pcur,parameter);
			}
			else if(strcmp(command,"test_user_passwd")==0)
			{
				thread_func_test_user_passwd(pcur,parameter);
			}
			else if(strcmp(command,"mkdir")==0)
			{
				thread_func_mkdir(pcur,parameter);
			}
			else
			{
				thread_func_fault(pcur);
			}
			free(pcur);
		}
	}
}
int thread_func_ls(pnode_t pnode)
{
	//	printf("now enter the thread_func_ls\n");
	func_log(pnode);
	struct dirent *pDirInfo;
	DIR *pDir;
	pDir=opendir(pnode->path);
	if(pDir==NULL)
	{
		printf("open dir fail!\n");
		return -1;
	}
	train t;
	char buf[1000]={0};
	while((pDirInfo=readdir(pDir))!=NULL)
	{
		sprintf(buf,"%2d------%s%s",pDirInfo->d_type,pDirInfo->d_name,"\n");
		t.model=1;
		t.len=strlen(buf);
		strcpy(t.buf,buf);
		send_n(pnode->new_fd,(char*)&t,8+t.len);
		bzero(buf,sizeof(buf));

	}
	t.model=1;
	t.len=0;
	send_n(pnode->new_fd,(char*)&t,8+t.len);//发送结束标示
	return 0;
}
int thread_func_pwd(pnode_t pnode)
{
	//	printf("now enter the thread_func_pwd\n");
	func_log(pnode);
	train t;
	sprintf(t.buf,"%s%s",pnode->path,"\n");
	t.model=1;
	t.len=strlen(t.buf);
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	t.model=1;
	t.len=0;
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	return 0;
}
int thread_func_cd(pnode_t pnode,char *parameter)
{
	func_log(pnode);
	int i,j;
	for(i=0,j=0;pnode->path[i];i++)
	{
		if(pnode->path[i]=='/')
		{
			j++;
		}
	}//j统计'/'出现的次数
	train t;
	if((j==1&&strcmp(parameter,"..")==0)||strcmp(parameter,".")==0)
	{
		strcpy(t.buf,pnode->path);
		goto end;
	}
	char parameter_path[1000]={0};
	sprintf(parameter_path,"%s%s%s",pnode->path,"/",parameter);
	if(opendir(parameter_path)==NULL)
	{
		strcpy(t.buf,pnode->path);
		goto end;
	}
	if(strcmp(parameter,"..")==0)
	{
		char buf[1000]={0};
		strcpy(buf,pnode->path);
		int buf_len=strlen(buf);
		for(;buf[buf_len-1]!='/';buf_len--)
		{
			buf[buf_len-1]=0;
		}
		buf[buf_len-1]=0;
		strcpy(t.buf,buf);
		//		printf("when parameter=..   path=%s\n",t.buf);
	}
	else
	{
		sprintf(t.buf,"%s%s%s",pnode->path,"/",parameter);
	}
	//	printf("path is change to %s\n",t.buf);
end:	t.model=2;
		t.len=strlen(t.buf);
		send_n(pnode->new_fd,(char*)&t,8+t.len);
		t.model=1;
		t.len=0;
		send_n(pnode->new_fd,(char*)&t,8+t.len);
		return 0;
}
int thread_func_remove(pnode_t pnode,char *parameter)
{
	//	printf("now enter the thread_func_remove\n");
	func_log(pnode);
	char buf[1000]={0};
	sprintf(buf,"%s%s%s",pnode->path,"/",parameter);
	remove(buf);
	train t;
	t.model=1;
	t.len=0;
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	return 0;
}
int thread_func_gets(pnode_t pnode,char*parameter)
{
	//	printf("now enter the thread_func_gets\n");
	func_log(pnode);
	int fd;
	char parameter_path[1000]={0};
	char *pmmap;
	sprintf(parameter_path,"%s%s%s",pnode->path,"/",parameter);
	if((fd=open(parameter_path,O_RDONLY))==-1)
	{
		thread_func_fault(pnode);
		return -1;
	}
	else
	{
		train t;
		//先发文件名
		t.model=3;
		t.len=strlen(parameter);
		strcpy(t.buf,parameter);
		int ret;
		ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
		if(-1==ret)
		{
			goto end;
		}
		//发文件大小
		struct stat buf;
		fstat(fd,&buf);
		t.len=sizeof(buf.st_size);
		memcpy(t.buf,&buf.st_size,sizeof(off_t));
		t.model=3;
		ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
		if(-1==ret)
		{
			goto end;
		}
		//发文件内容
		int file_cur_addr=0;
		int size_mmap=4096;
		if(buf.st_size>100*1024*1024)
		{
			printf("mmap train file\n");
			while(file_cur_addr+size_mmap<buf.st_size)
			{
				printf("enter while\n");
			pmmap=(char*)mmap(NULL,size_mmap,PROT_READ,MAP_SHARED,fd,file_cur_addr);
			perror("mmap");
			printf("mmap OK,size_mmap=%d,file_cur_addr=%d,sizeof(t.buf)=%ld\n",size_mmap,file_cur_addr,sizeof(t.buf));
			memcpy(t.buf,pmmap,size_mmap);
			printf("memcpy OK\n");
			t.model=3;
			t.len=size_mmap;
			ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
			file_cur_addr+=size_mmap;
			}
			printf("退出循环\n");
			pmmap=(char*)mmap(NULL,buf.st_size-file_cur_addr,PROT_READ,MAP_SHARED,fd,file_cur_addr);
			memcpy(t.buf,pmmap,buf.st_size-file_cur_addr);
			t.model=3;
			t.len=buf.st_size-file_cur_addr;
			ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
			munmap(pmmap,buf.st_size-file_cur_addr);
		}else
		{
		while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
		{
			t.model=3;
			ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
			if(-1==ret)
			{
				goto end;
			}
		}
		}
		t.model=1;
		t.len=0;
		ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
		if(-1==ret)
		{
			goto end;
		}
	}
end:
	return 0;
}

int thread_func_puts(pnode_t pnode,char*parameter)
{
	printf("now enter the thread_func_puts\n");
	func_log(pnode);
	train t;
	t.model=4;
	strcpy(t.buf,parameter);
	t.len=strlen(t.buf);
	send_n(pnode->new_fd,(char*)&t,8+t.len);//发送消息告知接收文件
	//另起一个端口号2001用于传输文件
	int sfd;
	tcp_start_listen(&sfd,"192.168.5.123","2001",100);
	printf("tcp_start_listen finish\n");
	int new_sfd;
	printf("开始accept\n");
	new_sfd=accept(sfd,NULL,NULL);
	printf("accept 成功，开始用新端口传输\n");
	//开始接收
	int model,len;
	char buf[1000]={0};
	recv_n(new_sfd,(char*)&model,4);
	//接文件名
	recv_n(new_sfd,(char*)&len,4);
	recv_n(new_sfd,buf,len);
	//接文件大小
	off_t file_size;
	double down_load_size=0;
	recv_n(new_sfd,(char*)&model,4);
	recv_n(new_sfd,(char*)&len,4);
	recv_n(new_sfd,(char*)&file_size,len);
	char parameter_path[1000]={0};
	sprintf(parameter_path,"%s%s%s",pnode->path,"/",buf);
	puts(parameter_path);
	int fd=open(parameter_path,O_RDWR|O_CREAT,0666);
	check_error(-1,fd,"open");
	//按大小打印下载百分比
	off_t compare_size=file_size/100;
	int ret;
	while(1)
	{
		recv_n(new_sfd,(char*)&model,4);
		ret=recv_n(new_sfd,(char*)&len,4);
		if(ret!=-1&&len>0)
		{
			ret=recv_n(new_sfd,buf,len);
			if(ret==-1)
			{
				printf("down percent %5.2f%s\n",down_load_size/file_size*100,"%");
				break;
			}
			write(fd,buf,len);
			down_load_size=down_load_size+len;
			if(down_load_size>compare_size)
			{
				printf("down percent %5.2f%s\r",down_load_size/file_size*100,"%");
				fflush(stdout);
				compare_size=compare_size+file_size/100;
			}
		}
		else
		{
			printf("down percent %5.2f%s\n",down_load_size/file_size*100,"%");
			break;
		}
	}
	close(new_sfd);
	close(sfd);
	close(fd);
	return 0;
}
int thread_func_fault(pnode_t pnode)
{
	//	printf("now enter the thread_func_fault\n");
	func_log(pnode);
	train t;
	t.model=1;
	strcpy(t.buf,"命令有误，重新输入\n");
	t.len=strlen(t.buf);
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	t.len=0;
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	return 0;
}
int thread_func_test_user_name(pnode_t pnode,char*parameter)
{
	//	printf("now enter the thread_func_test_user_name,order=%s,parameter=%s,len=%ld\n",pnode->order,parameter,strlen(parameter));
	int i,j;
	train t;
	struct spwd*sp;
	char salt[100]={0};
	sp=getspnam(parameter);
	if(sp==NULL)
	{

		//	printf("getspnam fail\n");
		thread_func_fault(pnode);
		return 0;
	}
	//	printf("getspnam OK\n");
	for(i=0,j=0;sp->sp_pwdp[i]&&j!=3;i++)//取得salt值
	{
		//	printf("进入for循环\n");
		if(sp->sp_pwdp[i]=='$')
		{
			++j;
		}
	}
	//		printf("before strncpy\n");
	strncpy(salt,sp->sp_pwdp,i-1);
	//		printf("after strncpy\n");
	strcpy(t.buf,salt);
	//	printf("salt=%s\n",salt);
	t.model=5;
	t.len=strlen(t.buf);
	send_n(pnode->new_fd,(char*)&t,8+t.len);//传回盐值
	//	printf("send ret=%d\n",ret);
	return 0;
}
int thread_func_test_user_passwd(pnode_t pnode,char*parameter)
{
	train t;
	//	printf("now enter the thread_func_test_user_passwd,parameter=%s\n",parameter);
	int i,j;
	char parameter_1[1000]={0};
	char parameter_2[1000]={0};
	for(i=0;i<strlen(parameter)&&parameter[i]!=' ';i++)
	{
		parameter_1[i]=parameter[i];//参数一为秘钥
	}
	//	printf("parameter_1=%s\n",parameter_1);
	i++;
	for(j=0;i<strlen(parameter);i++,j++)
	{
		parameter_2[j]=parameter[i];//第二个参数为用户名
	}
	//	printf("parameter_2=%s\n",parameter_2);
	struct spwd*sp;
	sp=getspnam(parameter_2);
	if(sp!=NULL)
	{
		//	printf("getspnam OK\n");
	}
	if(strcmp(parameter_1,sp->sp_pwdp)==0)
	{
		//		printf("strcmp OK\n");
		strcpy(t.buf,"OK");
		int func_enter_log(pnode_t);
	}
	else
	{
		//		printf("strcmp NO\n");
		strcpy(t.buf,"NO");
	}
	t.len=strlen(t.buf);
	t.model=5;
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	func_enter_log(pnode);
	//	printf("send_n ret=%d,t.len=%d,t.buf=%s,\n",ret,t.len,t.buf);
	return 0;
}
int thread_func_mkdir(pnode_t pnode,char*parameter)
{
	//	printf("now enter the thread_func_mkdir,parameter is%s,len=%ld\n",parameter,strlen(parameter));
	int func_log(pnode_t);
	train t;
	char parameter_path[1000]={0};
	sprintf(parameter_path,"%s%s%s",pnode->path,"/",parameter);
	if(opendir(parameter_path)!=NULL)
	{
		strcpy(t.buf,"directory list is exist");
	}
	else
	{
		mkdir(parameter_path,0777);
		strcpy(t.buf,"mkdir OK");
	}
	t.model=1;
	t.len=strlen(t.buf);
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	t.model=1;
	t.len=0;
	send_n(pnode->new_fd,(char*)&t,8+t.len);
	return 0;
}
int thread_func_gets_continue(pnode_t pnode,char*parameter)
{
	printf("now enter the thread_func_gets_continue,parameter is%s,len=%ld\n",parameter,strlen(parameter));
	func_log(pnode);
	int i,j;
	char parameter_1[1000]={0};
	char parameter_2[1000]={0};
	for(i=0;i<strlen(parameter)&&parameter[i]!=' ';i++)
	{
		parameter_1[i]=parameter[i];//参数一为文件名
	}
	i++;
	for(j=0;i<strlen(parameter);i++,j++)
	{
		parameter_2[j]=parameter[i];//第二个参数为已经下载的文件大小
	}
	//	printf("parameter_2=%s\n",parameter_2);
	int fd;
	char parameter_path[1000]={0};
	sprintf(parameter_path,"%s%s%s",pnode->path,"/",parameter);
	if((fd=open(parameter_path,O_RDONLY))==-1)
	{
		thread_func_fault(pnode);
		return -1;
	}
	else
	{
		train t;
		//先发文件名
		t.model=5;
		t.len=strlen(parameter_1);
		strcpy(t.buf,parameter_1);
		int ret;
		ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
		if(-1==ret)
		{
			goto end;
		}
		//发文件大小
		struct stat buf;
		fstat(fd,&buf);
		t.len=sizeof(buf.st_size);
		memcpy(t.buf,&buf.st_size,sizeof(off_t));
		t.model=5;
		ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
		if(-1==ret)
		{
			goto end;
		}
		//发文件内容
		FILE*fp=fopen(parameter_path,"rb+");
		ret=fseek(fp,atoi(parameter_2),SEEK_SET);
		while((t.len=fread(t.buf,1,sizeof(t.buf),fp))>0)
		{
			t.model=5;
			ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
			if(-1==ret)
			{
				goto end;
			}
		}
		t.model=1;
		t.len=0;
		ret=send_n(pnode->new_fd,(char*)&t,8+t.len);
		if(-1==ret)
		{
			goto end;
		}
	}
end:
	return 0;
}
