#include "func.h"

int main()
{
	int sfd;
	//初始化socket
	sfd=socket(AF_INET,SOCK_STREAM,0);
	check_error(-1,sfd,"socket");
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi("2000"));//端口号转换为网络字节序
	ser.sin_addr.s_addr=inet_addr("192.168.5.123");//点分10进制的IP地址转为网络字节序
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	check_error(-1,ret,"connect");
	train t;
	char order[1000]={0};
	char path[100]=".";
	char buf[1000]={0};
	int len,model;
	char command[50]={0};
	char parameter[50]={0};
	int i,j,fd;
	char user_name[100]={0};
	char* user_passwd;
	char user_passwd_salt[100]={0};
	char salt[512]={0};
	char order_test_user_name[100]="test_user_name";
	char order_test_user_passwd[100]="test_user_passwd";
	struct stat file_stat;
	//用户登录验证
	while(1)
	{
		bzero(user_name,sizeof(user_name));
		bzero(salt,sizeof(salt));
		bzero(t.buf,sizeof(t.buf));
		printf("input your name:\n");
		read(STDIN_FILENO,user_name,sizeof(user_name));
		user_passwd=getpass("请输入密码:\n");
		for(i=100;i-100<strlen(order_test_user_name);i++)
		{
			t.buf[i]=order_test_user_name[i-100];//初始化命令为test_user_name
		}
		t.buf[i++]=' ';
		for(j=0;j<strlen(user_name);i++,j++)
		{
			t.buf[i]=user_name[j];
		}
		t.model=5;
		t.len=i-1;//不要回车符
		send_n(sfd,(char*)&t,8+t.len);//发送用户名
		//		printf("发送用户名的buf=%s,返回值ret=%d,sizeof =%d\n",buf,ret,t.len+8);
		recv_n(sfd,(char*)&model,4);
		if(model==1)
		{
			printf("用户名有错，重新输入\n");
			continue;
		}
		//输入用户名成功后初始化路径
		sprintf(path,"%s%s%s",path,"/",user_name);
		path[strlen(path)-1]='\0';
		recv_n(sfd,(char*)&len,4);
		recv_n(sfd,salt,len);//接收服务器返回的盐值
		//		printf("接收服务器返回的盐值=%s\n",salt);
		strcpy(user_passwd_salt,crypt(user_passwd,salt));
		bzero(t.buf,sizeof(t.buf));
		strcpy(t.buf,path);
		for(i=100;i-100<strlen(order_test_user_passwd);i++)
		{
			t.buf[i]=order_test_user_passwd[i-100];//发送的order_test_user_passwd命令
		}
		//		printf("after 发送order_test_user_passwd命令i=%d\n",i);
		t.buf[i++]=' ';
		for(j=0;j<strlen(user_passwd_salt);j++,i++)
		{
			t.buf[i]=user_passwd_salt[j];
		}
		//		printf("after 发送user_passwd_salt命令i=%d\n",i);
		t.buf[i++]=' ';
		for(j=0;j<strlen(user_name);i++,j++)
		{
			t.buf[i]=user_name[j];
		}
		//		printf("after 发送user_name命令i=%d\n",i);
		//		printf("发送过去的秘钥=%s,以及用户名=%s,用户名len=%ld\n",user_passwd_salt,user_name,strlen(user_name));
		t.model=5;
		t.len=i-1;//不要回车符
		send_n(sfd,(char*)&t,8+t.len);//发送过去的第一个参数为秘钥，第二个参数为用户名
		//		printf("发送过去的第一个参数为秘钥,第二个参数为用户名,len=%d\n",8+t.len);
		bzero(buf,sizeof(buf));
		recv_n(sfd,(char*)&model,4);
		recv_n(sfd,(char*)&len,4);
		recv_n(sfd,buf,len);
		//		printf("recv buf=%s,len=%d\n",buf,len);
		if(strcmp(buf,"OK")==0)
		{
			printf("登陆成功\n");
			break;
		}
		else
		{
			printf("用户名或密码错误，请重新输入\n");
		}
	}
	//用户输入命令
	while(1)
	{
		fflush(stdout);
		fflush(stdin);
		bzero(order,sizeof(order));
		bzero(command,sizeof(command));
		bzero(parameter,sizeof(parameter));
		read(STDIN_FILENO,order,sizeof(order));
		for(i=0;order[i]!=' '&&i<strlen(order);i++,j++)
		{
			command[i]=order[i];
		}
		command[i-1]='\0';
		i++;
		for(j=0;order[i]!=' '&&i<strlen(order);i++,j++)
		{
			parameter[j]=order[i];
		}
		parameter[j-1]='\0';//不要回车符
		//	int parameter_len=strlen(parameter);
		//	printf("client command=%s,parameter=%s,parameter_len=%d\n",command,parameter,parameter_len);
		if(strcmp(command,"puts")==0)
		{
			if((fd=open(parameter,O_RDONLY))==-1)
			{
				printf("no this file\n");
				continue;
			}
		}else if(strcmp(command,"gets")==0)
		{
			if((fd=open(parameter,O_RDONLY))!=-1)
			{
				fstat(fd,&file_stat);
				sprintf(order,"%s%s%s%ld%s","gets_continue ",parameter," ",file_stat.st_size," ");
				close(fd);
			}
		}
		t.model=2;
		t.len=strlen(order);//键入的命令长度
		strcpy(t.buf,path);//拼接路径
		int i;
		for(i=100;i-100<t.len;i++)
		{
			t.buf[i]=order[i-100];//拼接命令
		}
		t.len=t.len+100-1;//不要回车符
		ret=send_n(sfd,(char*)&t,8+t.len);
		//		if(ret!=-1)
		//		{
		//			printf("send OK,发送过去的是%s\n",order);
		//		}
		//		system("clear");//清屏
		while(1)
		{
			bzero(buf,sizeof(buf));
			recv_n(sfd,(char*)&model,4);
			if(model==1)
			{
				ret=recv_n(sfd,(char*)&len,4);
				if(ret!=-1&&len>0)
				{
					recv_n(sfd,buf,len);
					printf("%s",buf);
				}
				else
				{
					//					printf("退出接收循环\n");
					break;
				}
			}
			else if(model==2)
			{
				ret=recv_n(sfd,(char*)&len,4);
				if(ret!=-1&&len>0)
				{
					recv_n(sfd,buf,len);
					bzero(path,sizeof(path));
					strcpy(path,buf);
					printf("client path is %s\n",path);
				}
				else
				{
					//					printf("退出路径循环\n");
					break;
				}
			}
			else if(model==3)
			{
				//接文件名
				recv_n(sfd,(char*)&len,4);
				recv_n(sfd,buf,len);
				//接文件大小
				off_t file_size;
				double down_load_size=0;
				recv_n(sfd,(char*)&model,4);
				recv_n(sfd,(char*)&len,4);
				recv_n(sfd,(char*)&file_size,len);
				fd=open(buf,O_RDWR|O_CREAT,0666);
				if(fd==-1)
				{
					printf("wrong:open file fault\n");
					break;
				}
				//按大小打印下载百分比
				off_t compare_size=file_size/100;
				while(1)
				{
					recv_n(sfd,(char*)&model,4);
					ret=recv_n(sfd,(char*)&len,4);
					if(ret!=-1&&len>0)
					{
						ret=recv_n(sfd,buf,len);
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
				close(fd);
				//				printf("退出下载循环\n");
				break;
			}
			else if(model==4)
			{
				printf("client enter model 4,puts file\n");
				recv_n(sfd,(char*)&len,4);
				bzero(buf,sizeof(buf));
				recv_n(sfd,buf,len);//接受了上传的文件名在buf里
				//另起一个端口发送
				int new_sfd;
				//初始化socket
				new_sfd=socket(AF_INET,SOCK_STREAM,0);
				if(new_sfd==-1)
				{
					printf("create the second socket fail\n");
					break;
				}
				struct sockaddr_in new_ser;
				bzero(&new_ser,sizeof(new_ser));
				new_ser.sin_family=AF_INET;
				new_ser.sin_port=htons(atoi("2001"));//端口号转换为网络字节序
				new_ser.sin_addr.s_addr=inet_addr("192.168.5.123");//点分10进制的IP地址转为网络字节序
				ret=connect(new_sfd,(struct sockaddr*)&new_ser,sizeof(struct sockaddr));
				if(ret==-1)
				{
					printf("create second connet fail\n");
					break;
				}
				printf("now connect OK,ret=%d,new_sfd=%d\n",ret,new_sfd);
				//先发文件名
				bzero(t.buf,sizeof(t.buf));
				t.model=4;
				t.len=strlen(buf);
				strcpy(t.buf,buf);
				ret=send_n(new_sfd,(char*)&t,8+t.len);
				if(-1==ret)
				{
					goto end;
				}
				printf("先发文件名OK\n");
				//发文件大小
				fd=open(buf,O_RDONLY);
				if(fd==-1)
				{
					printf("wrong:open file fault\n");
					break;
				}
				struct stat buff;
				fstat(fd,&buff);
				t.len=sizeof(buff.st_size);
				memcpy(t.buf,&buff.st_size,sizeof(off_t));
				t.model=4;
				ret=send_n(new_sfd,(char*)&t,8+t.len);
				if(-1==ret)
				{
					goto end;
				}
				//测试发送进度
				off_t file_size=buff.st_size;
				printf("file_size=%ld\n",file_size);
				double send_size=0;
				off_t compare_size=file_size/100;
				//发文件内容
				while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
				{
					t.model=4;
					ret=send_n(new_sfd,(char*)&t,8+t.len);
					if(-1==ret)
					{
						printf("send percent %5.2f%s\n",send_size/file_size*100,"%");
						goto end;
					}
					send_size=send_size+t.len;
					if(send_size>compare_size)
					{
						printf("send percent %5.2f%s\r",send_size/file_size*100,"%");
						fflush(stdout);
						compare_size=compare_size+file_size/100;
					}
				}
				printf("send percent %5.2f%s\n",send_size/file_size*100,"%");
				t.model=1;
				t.len=0;
				ret=send_n(new_sfd,(char*)&t,8+t.len);
				if(-1==ret)
				{
					goto end;
				}
end:
				close(new_sfd);
				close(fd);
				break;
			}else if(model==5)//断点续传
			{
				//接文件名
				recv_n(sfd,(char*)&len,4);
				recv_n(sfd,buf,len);
				//接文件大小
				off_t file_size;
				double down_load_size=0;
				recv_n(sfd,(char*)&model,4);
				recv_n(sfd,(char*)&len,4);
				recv_n(sfd,(char*)&file_size,len);
				fd=open(buf,O_RDWR|O_CREAT|O_APPEND,0666);
				if(fd==-1)
				{
					printf("wrong:open file fault\n");
					break;
				}
				fstat(fd,&file_stat);
				down_load_size=file_stat.st_size;
				//按大小打印下载百分比
				off_t compare_size=file_size/100;
				FILE*fp=fopen(buf,"wb+");
				fseek(fp,file_stat.st_size,SEEK_SET);
				while(1)
				{
					recv_n(sfd,(char*)&model,4);
					ret=recv_n(sfd,(char*)&len,4);
					if(ret!=-1&&len>0)
					{
						ret=recv_n(sfd,buf,len);
						if(ret==-1)
						{
							printf("down percent %5.2f%s\n",down_load_size/file_size*100,"%");
							break;
						}
						fwrite(buf,1,len,fp);
						down_load_size=down_load_size+len;
						if(down_load_size>compare_size)
						{
							printf("down percent %5.2f%s\r",down_load_size/file_size*100,"%");
							fflush(stdout);
							compare_size=compare_size+file_size/100;
						}
					}else 
					{
						printf("down percent %5.2f%s\n",down_load_size/file_size*100,"%");
						break;
					}
				}
				close(fd);
				break;
			}
		}
	}
	close(sfd);
}
