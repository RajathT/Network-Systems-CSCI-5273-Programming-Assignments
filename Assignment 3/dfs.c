#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<fcntl.h>
#include <dirent.h> 

#define USERS 2
char *users[USERS];
char *pass[USERS];
char *myName;
char* curruser;
int fileinput;

void readConf(char* path){
	//printf("Hello: %s\n",myName);
	FILE *fd;int i=0,count=0;
	char* ConfFilePath = (char *)malloc(200);
	//ConfFilePath=getenv("PWD");
	strcpy(ConfFilePath,path);
	strncat(ConfFilePath,"/dfs.conf",9);
	//printf("%s\n",ConfFilePath);
	
	fd = fopen(ConfFilePath,"r");
	fseek(fd,0,SEEK_END);
	int filesize=ftell(fd);
	fseek(fd,0,SEEK_SET);
	
	char *fileContent=(char *)malloc(200);
	char *temp=(char *)malloc(100);
	
	while(fgets(fileContent,filesize,fd)!=NULL){
		//printf("File Content :%s\n",fileContent);
		
		if(i<USERS){
			
			temp=strtok(fileContent," \t\n");
			//printf("TEMP: %s\n",temp);
			fileContent=strtok(NULL, " \t\n");
			//printf("PASS: %s\n",fileContent);
			users[i]=(char*)malloc(100);
			pass[i]=(char*)malloc(100);
			
			strcpy(users[i],temp);
			strcpy(pass[i],fileContent);
			//printf("HELLO User: %s\n",users[i]);
			//printf("HELLO Pass: %s\n",pass[i]);
			i++;
		}
		
		
		
	}
	i=0;
	while (i<USERS){
		//printf("%s %s\n",users[i],pass[i]);
		i++;
	}
	fclose(fd);
}
int validateUser(int cli,char *check){
	char *userr=(char*)malloc(100);
	char *passs=(char*)malloc(100);
	curruser=(char*)malloc(100);
	int i,flag=0;
	userr = strtok(check," ");
	passs = strtok(NULL," ");
	//printf("HELLO!!!!!!!!!!!!!!!!!!!!!!: %s %s %s\n",userr,passs,myName);
	if(flag==0){
	printf("Coming into first Flag\n");
	for(i=0;i<USERS;i++){
		if(strcmp(users[i],userr)==0&&strcmp(pass[i],passs)==0){
			printf("Success USERNAME and PASSWORDS match!\n");
			char *errorr = (char*)malloc(100);
			bzero(errorr,sizeof(errorr));
			strcpy(errorr,myName);
			strcpy(curruser,userr);
			//printf("Error: %s\n",errorr);
			strcat(errorr,"Success");
			//printf("Message: %s\n",errorr);
			send(cli,errorr,strlen(errorr),0);
			return 1;
			flag=1;
		}
	}
	}
	
	if(flag==0){
		printf("Coming into second flag %s %s\n",userr,passs);
		printf("Error! Invalid Credentials!\n");
		char *error = (char*)malloc(100);
		bzero(error,sizeof(error));
		strcpy(error,myName);
		strcat(error,"ERROR: Invalid Credentials");
		send(cli,error,strlen(error),0);
		return -1;
	}
	
}
int getFileFromClient(char*pathe,int client){
	char *authen=(char*)malloc(2048);
	bzero(authen,sizeof(authen));
	int n = recv(client,authen,2048,0);
	printf("AUTHENNNNNNNNNN: %s\n",authen);
	int check=validateUser(client,authen);
	//printf("#####%s\n",pathe);
	if(check==-1)
		return check;
	else{
		char* filepath=(char*)malloc(600);
		char* checker=(char*)malloc(600);
		strcpy(checker,pathe);
		strcat(checker,"/");
		strcat(checker,myName);
		FILE *fd;
		fd=fopen(checker,"r");
		if(fd==NULL){
			strcpy(filepath,"mkdir ");
		//strcpy(filepath,pathe);
		//strcat(filepath,"/");
			strcat(filepath,myName);
			//printf("%s\n",filepath);
			system(filepath);
		}
		//printf("Im hereasdfasdasdf\n");
		char *filename=(char*)malloc(50);
		recv(client,filename,50,0);
		fileinput=strlen(filename);
		//printf("!!!!!!!!!!!!!!!##%s %d\n",filename,strlen(filename));
		if(filename[0]=='.'){
			char*filen=strtok(filename," ");
			printf("File Recieved: %s\n",filen);
			char *files=strtok(NULL," ");
			char *suby=strtok(NULL," ");
			printf("Sub Folder to put: %s\n",suby);
			//printf("%s %s\n",filen,files);
			strcpy(filename,filen);
			strcpy(filepath,"mkdir -p ");
			strcat(filepath,myName);
			strcat(filepath,"/");
			strcat(filepath,curruser);
			if(strcmp(suby,"NULL")!=0){
				strcat(filepath,"/");
				strcat(filepath,suby);
			}
			//printf("!&&&#%s\n",filepath);
			system(filepath);
			//char *filesize=(char*)malloc(10);
			//bzero(filesize,sizeof(filesize));*/
			//recv(client,filesize,4,0);
			int filer1=atoi(files);
			//printf("FILEEEEEESIZEEEE: %d\n",filer1);
			int l;
			if(filer1>0){
				char *filedata1=(char*)malloc(filer1);
				int byters=0;
				
				byters=recv(client,filedata1,filer1,0);
					
				if(byters){
					//printf("We got the bytes%d\n",byters);
					
					char *fileopen;
					strtok(filepath," ");
					fileopen=strtok(NULL," ");
					fileopen=strtok(NULL," ");
					//printf("%s\n",fileopen);
					char *mainopen=(char*)malloc(strlen(fileopen)+strlen(filen)+2+10);
					strcpy(mainopen,".");
					strcat(mainopen,"/");
					strcat(mainopen,fileopen);
					strcat(mainopen,"/");
					strcat(mainopen,filen);
					//printf("LLLLLLLLL:::%s\n",mainopen);
					FILE *f1;
					//mainopen="./DFS1/.exam.gif.1";
					f1=fopen(mainopen,"w");
					if(f1==NULL){
						//printf("Lauda\n");
					}
					else{
						//printf("Heyyyy\n");
						fwrite(filedata1,filer1,1,f1);
						fclose(f1);
					}
				}
			}
			
		
		}
		
		
		//printf("Ready to recieve!!");
		
		//printf("Im hereasdfasdasdf\n");
		char *filename1=(char*)malloc(fileinput);
		recv(client,filename1,fileinput,0);
		//printf("!!!!!!!!!!!!!!!##%s\n",filename1);
		if(filename1[0]=='.'){
			//printf("File2 da!\n");
			char*filen1=strtok(filename1," ");
			printf("File Recieved: %s\n",filen1);
			char *files1=strtok(NULL," ");
			char *suby1=strtok(NULL," ");
			printf("Sub folder to put: %s\n",suby1);
			//printf("%s %s\n",filen1,files1);
			strcpy(filename1,filen1);
			strcpy(filepath,"mkdir -p ");
			strcat(filepath,myName);
			strcat(filepath,"/");
			strcat(filepath,curruser);
			if(strcmp(suby1,"NULL")!=0){
				strcat(filepath,"/");
				strcat(filepath,suby1);
			}
			//printf("!&&&#%s\n",filepath);
			system(filepath);
			//char *filesize=(char*)malloc(10);
			//bzero(filesize,sizeof(filesize));
			//recv(client,filesize,4,0);
			int filer2=atoi(files1);
			//printf("FILEEEEEESIZEEEE: %d\n",filer2);
			
			if(filer2>0){
			//printf("filer2\n");
				char *filedata2=(char*)malloc(filer2);
				int byters1=0;
				
				byters1=recv(client,filedata2,filer2,0);
					
				if(byters1){
					//printf("We got the bytes%d\n",byters1);
					char *fileopen1;
					strtok(filepath," ");
					fileopen1=strtok(NULL," ");
					fileopen1=strtok(NULL," ");
					//printf("%s\n",fileopen1);
					char *mainopen1=(char*)malloc(strlen(fileopen1)+strlen(filen1)+2+10);
					strcpy(mainopen1,".");
					strcat(mainopen1,"/");
					strcat(mainopen1,fileopen1);
					strcat(mainopen1,"/");
					strcat(mainopen1,filen1);
					//printf("LLLLLLLLL:::%s\n",mainopen1);
					FILE *f2;
					//mainopen="./DFS1/.exam.gif.1";
					f2=fopen(mainopen1,"w");
					if(f2==NULL){
						//printf("Lauda\n");
					}
					else{
						//printf("Heyyyy\n");
						fwrite(filedata2,filer2,1,f2);
						fclose(f2);
					}
				}
			}
			//recv(client,)*/
		
		}
		return 1;
	}
}
int sendFileToClient(char *pather,int clie){
	//printf("Hello!");
	int by,i;
	char *authen=(char*)malloc(2048);
	bzero(authen,sizeof(authen));
	
	int n = recv(clie,authen,2048,0);
	printf("AUTHENNNNNNN: %s\n",authen);
	int check=validateUser(clie,authen);
	if(check==-1)
		return check;
	else{
		//printf("Hello!\n");
		char *data=(char*)malloc(100);
		bzero(data,sizeof(data));
		by=recv(clie,data,100,0);
		//printf("KEvvvvvvvvvv%s\n",data);
		char *fi=strtok(data," ");
		char *su=strtok(NULL," ");
		char* command = (char*) malloc(strlen(fi));
		strcpy(command,fi);
		char* command1 = (char*) malloc(strlen(su));
		strcpy(command1,su);
		//strcat(command, myName);
		printf("File to send: %s in subfolder %s\n",command,command1);
		char *filename1 = (char*)malloc(strlen(command)+3);
		sprintf(filename1,".%s.%d",command,1);
		//printf("%s\n",filename1);
		char *filename2 = (char*)malloc(strlen(command)+3);
		sprintf(filename2,".%s.%d",command,2);
		//printf("%s\n",filename2);
		char *filename3 = (char*)malloc(strlen(command)+3);
		sprintf(filename3,".%s.%d",command,3);
		//printf("%s\n",filename3);
		char *filename4 = (char*)malloc(strlen(command)+3);
		sprintf(filename4,".%s.%d",command,4);
		//printf("%s\n",filename4);
		
		char* path1=(char*)malloc(100);
		strcpy(path1,".");
		strcat(path1,"/");
		strcat(path1,myName);
		strcat(path1,"/");
		strcat(path1,curruser);
		if(strcmp(command1,"NULL")!=0){
			strcat(path1,"/");
			strcat(path1,command1);
		}
		strcat(path1,"/");
		strcat(path1,filename1);
		
		//printf("%s\n",path1);
		
		FILE *fr1;
		fr1=fopen(path1,"r");
		//printf("%d",fr1);
		if(fr1==NULL){
			//perror("Lauda");
		}
		else{
			printf("File Sent: %s\n",filename1);
			fseek(fr1,0,SEEK_END);
			int filesize1=ftell(fr1);
			//strcat(filename3," ");
			char*charfilesize1=(char*)malloc(50);
			bzero(charfilesize1,sizeof(charfilesize1));
			sprintf(charfilesize1,"%s %d ",filename1,filesize1);
			for(i=strlen(charfilesize1);i<50;i++)
				charfilesize1[i]='a';
			int tt=send(clie,charfilesize1,50,0);
			//printf("Fuck U! %d %s\n",tt,charfilesize1);
			fseek(fr1,0,SEEK_SET);
			char*thedata1=(char*)malloc(filesize1);
			tt=fread(thedata1,filesize1,1,fr1);
			//printf("%d\n",tt);
			send(clie,thedata1,filesize1,0);
		
			//printf("Fuck U!\n");
			fclose(fr1);
		}
		
		
		
		char* path2=(char*)malloc(100);
		strcpy(path2,".");
		strcat(path2,"/");
		strcat(path2,myName);
		strcat(path2,"/");
		strcat(path2,curruser);
		if(strcmp(command1,"NULL")!=0){
			strcat(path2,"/");
			strcat(path2,command1);
		}
		strcat(path2,"/");
		strcat(path2,filename2);
		
		//printf("%s\n",path2);
		
		FILE *fr2;
		fr2=fopen(path2,"r");
		if(fr2==NULL){
			//printf("Lauda");
		}
		else{	
			printf("File Sent: %s\n",filename2);
			fseek(fr2,0,SEEK_END);
			int filesize2=ftell(fr2);
			//strcat(filename3," ");
			char*charfilesize2=(char*)malloc(50);
			bzero(charfilesize2,sizeof(charfilesize2));
			sprintf(charfilesize2,"%s %d ",filename2,filesize2);
			for(i=strlen(charfilesize2);i<50;i++)
				charfilesize2[i]='a';
			int tt=send(clie,charfilesize2,50,0);
			//printf("Fuck U! %d %s\n",tt,charfilesize2);
			fseek(fr2,0,SEEK_SET);
			char*thedata=(char*)malloc(filesize2);
			tt=fread(thedata,filesize2,1,fr2);
			//printf("%d\n",tt);
			send(clie,thedata,filesize2,0);
			fclose(fr2);
		}
		
		
		
		char* path3=(char*)malloc(100);
		strcpy(path3,".");
		strcat(path3,"/");
		strcat(path3,myName);
		strcat(path3,"/");
		strcat(path3,curruser);
		if(strcmp(command1,"NULL")!=0){
			strcat(path3,"/");
			strcat(path3,command1);
		}
		strcat(path3,"/");
		strcat(path3,filename3);
		
		//printf("%s\n",path3);
		
		FILE *fr3;
		fr3=fopen(path3,"r");
		if(fr3==NULL){
			//printf("Lauda\n");
		}
		else{	
			printf("File Sent: %s\n",filename3);
			fseek(fr3,0,SEEK_END);
			int filesize3=ftell(fr3);
			//strcat(filename3," ");
			char*charfilesize3=(char*)malloc(50);
			bzero(charfilesize3,sizeof(charfilesize3));
			sprintf(charfilesize3,"%s %d ",filename3,filesize3);
			for(i=strlen(charfilesize3);i<50;i++)
				charfilesize3[i]='a';
			int tt=send(clie,charfilesize3,50,0);
			//printf("Fuck U! %d %s\n",tt,charfilesize3);
			fseek(fr3,0,SEEK_SET);
			char*thedata3=(char*)malloc(filesize3);
			tt=fread(thedata3,filesize3,1,fr3);
			//printf("%d\n",tt);
			send(clie,thedata3,filesize3,0);
			fclose(fr3);
		}
		
		
		
		char* path4=(char*)malloc(100);
		strcpy(path4,".");
		strcat(path4,"/");
		strcat(path4,myName);
		strcat(path4,"/");
		strcat(path4,curruser);
		if(strcmp(command1,"NULL")!=0){
			strcat(path4,"/");
			strcat(path4,command1);
		}
		strcat(path4,"/");
		strcat(path4,filename4);
		
		//printf("%s\n",path4);
		
		FILE *fr4;
		fr4=fopen(path4,"r");
		if(fr4==NULL){
			//printf("Lauda\n");
		}
		else{	
			printf("File Sent: %s\n",filename4);
			fseek(fr4,0,SEEK_END);
			int filesize4=ftell(fr4);
			//strcat(filename3," ");
			char*charfilesize4=(char*)malloc(50);
			bzero(charfilesize4,sizeof(charfilesize4));
			sprintf(charfilesize4,"%s %d ",filename4,filesize4);
			for(i=strlen(charfilesize4);i<50;i++)
				charfilesize4[i]='a';
			int tt=send(clie,charfilesize4,50,0);
			//printf("Fuck U! %d %s\n",tt,charfilesize4);
			fseek(fr4,0,SEEK_SET);
			char*thedata4=(char*)malloc(filesize4);
			tt=fread(thedata4,filesize4,4,fr4);
			//printf("%d\n",tt);
			send(clie,thedata4,filesize4,0);
		
			//printf("Fuck U!\n");
			fclose(fr4);
		}
		
		
		
	}
	
}
int sendListOfFiles(char *pat,int cli){
	printf("Sending List of Files\n");
	int flag=0;
	char *authen=(char*)malloc(2048);
	bzero(authen,sizeof(authen));
	int n = recv(cli,authen,2048,0);
	//printf("AUTHENNNNNNN: %s\n",authen);
	int check=validateUser(cli,authen);
	if(check==-1)
		return check;
	else{
		//printf("ImmmmmHello!\n");
		char *rew=(char*)malloc(20);
		recv(cli,rew,20,0);
		//printf("%s\n");
		char* subbe=(char*)malloc(10);
		subbe=strtok(rew, " ");
		DIR *d;
  		struct dirent *dir;
  		char* path = (char*)malloc(7);
  		strcpy(path,"./");
  		strcat(path,myName);
  		strcat(path,"/");
  		strcat(path,curruser);
  		if(strcmp(subbe,"NULL")!=0){
			strcat(path,"/");
			strcat(path,subbe);
		}
  		d = opendir(path);
  		char* content = (char*)malloc(200);
  		bzero(content,sizeof(content));
  		if (d){
    			while ((dir = readdir(d)) != NULL){
      				//printf("%s\n", dir->d_name);
      				if(flag==0){
      					strcpy(content,dir->d_name);
      					flag=1;
      				}
      				else{
      					strcat(content, " ");
      					strcat(content,dir->d_name);
      				}
    			}
    		closedir(d);
  		}
  		//printf("%s %d\n",content,strlen(content));
  		char* trash = (char*)malloc(5);
  		recv(cli,trash,5,0);
  		//printf("%s\n",trash);
  		send(cli,content,strlen(content),0);
	}
	
}
void checkForFile(char*pather,int client){
	//printf("CHECKFILEFUNC");
	while(1){
		//printf("Here");
		char *data=(char*)malloc(3);
		//printf("InsideL: %d\n",getpid());
		bzero(data,3);
		int n=recv(client,data,3,0);
		//printf("%s %d\n",data,n);
		if(n<1){
			close(client);
			exit(0);
		}
		
		if(strncmp(data,"get",3)==0||strncmp(data,"GET",3)==0){
			//printf("I'm gonna GET!\n");
			int grr=sendFileToClient(pather,client);
			if(grr==-1)
				break;
		}
		else if(strncmp(data,"put",3)==0||strncmp(data,"PUT",3)==0){
			printf("Ready to get the file!\n");
			int check=getFileFromClient(pather,client);
			if(check==-1)
				break;
			
		}
		else if(strncmp(data,"lis",3)==0||strncmp(data,"LIST",4)==0){
			//printf("I'm gonna LIST!\n");
			int bg=sendListOfFiles(pather,client);
			if(bg==-1)
				break;
		}
		//printf("%s %d",data,n);
		
		
		
		//close(client);
		//exit(0);
		//break;
	}

}
/*paste(char *filename){
	
}*/
int main(int argc, char *argv[]){
	struct sockaddr_in client_addr, server_addr;
	socklen_t sin_len = sizeof(client_addr);
	int fd_client, fd_server, filesize, port;
	
	char *filename = (char*)malloc(100);
	myName = (char*)malloc(100);
	
	strcpy(filename,argv[1]);
	strcpy(myName,argv[1]);
	//paste(filename);
	//myName=argv[1];
	//strcpy(myName,argv[1]);
	port=atoi(argv[2]);
	
	//printf("%s %s %d\n",myName,filename,port);
	
	char* path=(char*)malloc(600);
	//getcwd(path,sizeof(path));
	path=getenv("PWD");
	//printf("Fuck!!!!%s\n",path);
	//char* pathw=(char*)malloc(strlen(path));
	//strcpy(pathw,path);
	//printf("%s\n",pathw);
	readConf(path);
	//path=getenv("PWD");
	//printf("%s\n",path);
	fd_server = socket(AF_INET, SOCK_STREAM,0);
	
	if(fd_server < 0){
		perror("Socket");
		exit(1);
	}
	
	
	//setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	
	if(bind(fd_server,(struct sockaddr *) &server_addr,sizeof(server_addr)) == -1){
		perror("Bind");
		close(fd_server);
		exit(1);
	}
	
	if(listen(fd_server,10)==-1){
		perror("Listen");
		close(fd_server);
		exit(1);
	}
	
	//printf("%d\n",port);
	
	
	while(1){
		//printf("%d\n",port);
		fd_client = accept(fd_server, (struct sockaddr*) &client_addr, &sin_len);
		printf("Forking: %d %d\n",getpid(),fd_server);
		if(fd_client == -1){
			perror("Couldn't Accept");
			continue;
		}
		printf("Connected to Client \n");
		
		if(!fork()){
			
			checkForFile(path,fd_client);
			//printf("I'm here");
			close(fd_client);
			//exit(0);
		}
		//printf("!!I'm here");
		close(fd_client);
		
	}
	//printf("@@I'm here");
	close(fd_server);
	
	return 0;
}
