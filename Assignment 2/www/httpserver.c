#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<fcntl.h>
#define MAXSIZE 2048
char webpage[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html: charset=UTF-8\r\n\r\n";
/*"<!DOCTYPE html>\r\n"
"<html><head><title>Create Life</title>\r\n"
"<style>body {background-color: #FFFF00 }</style><head>\r\n"
"<body><center><h1>You can do it!!</h1><br></body></html>\r\n";*/
char *PORT;int PORT80;
char *buffers;
char *fileTypes[9];
char *fileExt[9];
char *defaultFile;
char *mainMethod;
char *postdata;
int nbytes, nnbytes;
int fd_server;
void readWS(){
	char *WSFilePath,*fileContent,*fileTy; int filesize;
	fileContent=(char*)malloc(MAXSIZE);
	defaultFile=(char*)malloc(MAXSIZE);
	fileTy=(char*)malloc(MAXSIZE);
	char* search=(char*)malloc(MAXSIZE);
	char *filext=(char*)malloc(MAXSIZE);
	
	FILE *fd;int i=0;
	for(i=0;i<9;i++){
		fileTypes[i]=(char*)malloc(MAXSIZE);
		fileExt[i]=(char*)malloc(MAXSIZE);
	}
	i=0;
	WSFilePath=getenv("PWD");
	//printf("%s\n",WSFilePath);
	strncat(WSFilePath,"/ws.conf",8);
	//printf("%s\n",WSFilePath);
	
	fd = fopen(WSFilePath,"r");
	fseek(fd,0,SEEK_END);
	filesize=ftell(fd);
	fseek(fd,0,SEEK_SET);
	if(fd<0){
		perror("FUCKK");
		//printf("FUCKK %d",filesize);
	}
	//fread(fileContent,MAXSIZE,1,fd);
	while(fgets(fileContent,filesize,fd)!=NULL){
		//printf("Main:%s\n",fileContent);
		if(!strncmp(fileContent,"Listen",6)){
			//printf("%s",fileContent);
			PORT=strtok(fileContent," ");
			PORT=strtok(NULL," ");
			//printf("%s",PORT);
			PORT80=atoi(PORT);
			//printf("%d",atoi(PORT));
			bzero(fileContent,sizeof(fileContent));
		}
		if(!strncmp(fileContent,"DocumentRoot",12)){
			//printf("%s\n",fileContent);
			search=strtok(fileContent," ");
			//printf("%s\n",defaultFile);
			search=strtok(NULL," \t\n");
			//printf("%s\n",defaultFile);
			strcpy(defaultFile,search);
			bzero(fileContent,sizeof(fileContent));
		}
		if(!strncmp(fileContent,".",1)){
			//printf("%s\n",fileContent);
			fileTy=strtok(fileContent," ");
			strcpy(fileTypes[i],fileTy);
			filext=strtok(NULL," \t\n");
			strcpy(fileExt[i],filext);
			i++;
			bzero(fileContent,sizeof(fileContent));
		}
	}
	//printf("Results********************\n");
	//printf("%d\n",PORT80);
	//printf("%s\n",defaultFile);
	//for(i=0;i<9;i++)
	//	printf("%s\n",fileTypes[i]);
	
}
int checkForExt(char *ext){
	int flag=0;int i,index;
	for(i=0;i<9;i++){
		//printf("Here %s %s %d\n",ext,fileTypes[i],flag);
		if(!strcmp(ext,fileTypes[i])){
			flag = 1;
			index=i;
			break;
		}
		else
			continue;
	}
	if(flag==1)
		return index;
	else
		return -1;
}
void sendFileToBrowser(char* filename,int fd_client,char *hversion,char *alive){
	//printf("$$$$$$$$$$$$$$$$$$$$$%s",filename);
	char *sendHeader = (char *)malloc(MAXSIZE);
	bzero(sendHeader,sizeof(sendHeader));
	FILE *fd,*fd1;int go=0,i,ny;
	char *bh = strrchr(filename,'.');
	//printf("jjjjj%s",bh);
	if(bh==NULL){
		strncat(sendHeader,hversion,strlen(hversion));
                strncat(sendHeader," 400 Bad Request\n",strlen(" 400 Bad Request\n"));
                strncat(sendHeader,"Content-Type:none\n",strlen("Content-type:none\n"));
                strncat(sendHeader,"Content-Length:none\n",strlen("Content-Length:none\n"));
               	strncat(sendHeader,alive,strlen(alive));                    
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"<HEAD><TITLE>400 Bad Request</TITLE></HEAD>",strlen("<HEAD><TITLE>400 Bad Request</TITLE></HEAD>"));
                strncat(sendHeader,"<html><BODY>400 Bad Request: Invalid URI: ",strlen("<html><BODY>400 Bad Request: Invalid URI: "));
                strncat(sendHeader,filename,strlen(filename));
                strncat(sendHeader,"</BODY></html>",strlen("</BODY></html>"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
		send(fd_client, sendHeader, strlen(sendHeader)-1,0);
		close(fd_client);
		exit(0);
	
	}
	go=checkForExt(bh);
	//go=-1;
	if(go==-1){
		strncat(sendHeader,hversion,strlen(hversion));
                strncat(sendHeader," 501 Not Implemented\n",strlen(" 501 Not Implemented\n"));
                strncat(sendHeader,"Content-Type:none\n",strlen("Content-type:none\n"));
                strncat(sendHeader,"Content-Length:none\n",strlen("Content-Length:none\n"));
               	strncat(sendHeader,alive,strlen(alive));                    
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"<HEAD><TITLE>501 Not Implemented</TITLE></HEAD>",strlen("<HEAD><TITLE>501 Not Implemented</TITLE></HEAD>"));
                strncat(sendHeader,"<html><BODY>501 Not Implemented: File format not supported: ",strlen("<html><BODY>501 Not Implemented: File format not supported: "));
                strncat(sendHeader,bh,strlen(bh));
                strncat(sendHeader,"</BODY></html>",strlen("</BODY></html>"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
		send(fd_client, sendHeader, strlen(sendHeader)-1,0);
		close(fd_client);
		exit(0);
	}
	else{
		//for(i=0;i<9;i++)
		//printf("%s\n",fileExt[i]);
		//printf("%d\n",go);
		//printf("%s %d\n",fileExt[go],strlen(fileExt[go]));
		fd=fopen(filename,"rb");
		if(fd==NULL){
			//perror("File Not Found");
			strncat(sendHeader,hversion,strlen(hversion));
                strncat(sendHeader," 404 File Not Found\n",strlen(" 404 File Not Found\n"));
                strncat(sendHeader,"Content-Type:none\n",strlen("Content-type:none\n"));
                strncat(sendHeader,"Content-Length:none\n",strlen("Content-Length:none\n"));
               	strncat(sendHeader,alive,strlen(alive));                    
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"<HEAD><TITLE>404 File Not Found</TITLE></HEAD>",strlen("<HEAD><TITLE>404 File Not Found</TITLE></HEAD>"));
                strncat(sendHeader,"<html><BODY>404 File Not Found: ",strlen("<html><BODY>404 File Not Found: "));
                strncat(sendHeader,filename,strlen(filename));
                strncat(sendHeader,"</BODY></html>",strlen("</BODY></html>"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
		send(fd_client, sendHeader, strlen(sendHeader)-1,0);
		close(fd_client);
		exit(0);
			
		}
		else{
		fseek(fd,0,SEEK_END);
		int filesize=ftell(fd);
		rewind(fd);
		printf("The Filesize: %d\n",filesize);
		char* filecize=(char *)malloc(10);
		sprintf(filecize,"%d",filesize);
		//printf("The FileCize: %s\n",filecize);
		char *buffer = (char *)malloc(filesize);
		bzero(buffer,filesize);
		ny=fread(buffer,1,filesize,fd);
		//fclose(fd);
		//printf("The buffer: %d\n",ny);
		strncat(sendHeader,hversion,strlen(hversion));
		strncat(sendHeader," 200 OK\n",strlen(" 200 OK\n"));
	strncat(sendHeader,"Content-Type: ",strlen("Content-Type: "));
		strncat(sendHeader,fileExt[go],strlen(fileExt[go]));
		strncat(sendHeader,"\n",strlen("\n"));
	strncat(sendHeader,"Content-Lenght: ",strlen("Content-Lenght: "));
		strncat(sendHeader,filecize,strlen(filecize));
		strncat(sendHeader,"\n",strlen("\n"));
		strncat(sendHeader,alive,strlen(alive));                    
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                strncat(sendHeader,"\r\n",strlen("\r\n"));
                
		//printf("%s\n",sendHeader);
		//printf("%d",strlen(buffer));
		nbytes=send(fd_client,sendHeader,strlen(sendHeader),0);
		//printf("!!!!!!!!!!!!!!!%s",mainMethod);
		if(strncmp(mainMethod,"GET",3)==0)
			nnbytes=send(fd_client,buffer,ny,0);
		else{
			//nnbytes=send(fd_client,"Hello",strlen("Hello"),0);
			fd1=fopen("sendfile.txt","w+");
			//fseek(fd,0,SEEK_END);
			
			rewind(fd);
			//printf("The Filesize: %d\n",filesize);
			//char* filecize=(char *)malloc(10);
			//sprintf(filecize,"%d",filesize);
		//printf("The FileCize: %s\n",filecize);
			char *postbuffer = (char *)malloc(filesize);
			bzero(postbuffer,filesize);
			//ny=fread(buffer,1,filesize,fd);
			//printf("@@@@@@@@)))))))))): %d\n",ny);
			while(fgets(postbuffer,filesize,fd)!=NULL){
				fputs(postbuffer,fd1);
				if(strncmp(postbuffer,"<body>",6)==0){
					fputs("<pre><h1>",fd1);
					fputs(postdata,fd1);
					fputs("</pre></h1>",fd1);
				}
				//send(fd_client,postbuffer,sizeof(postbuffer),0);
			}
			fseek(fd1, 0, SEEK_SET);
			fseek(fd1,0,SEEK_END);
			int filesize1=ftell(fd1);
			//printf("@@@@@@@@@@@@@@@@@@@@The Filesize: %d\n",filesize1);
			fseek(fd1, 0, SEEK_SET);
			char *buffer1 = (char *)malloc(filesize1);
			bzero(buffer1,filesize1);
			int ny1=fread(buffer1,1,filesize1,fd1);
			nnbytes=send(fd_client,buffer1,ny1,0);
			//nnbytes=send(fd_client,"Hello",strlen("Hello"),0);
			//fclose(fd1);
			//fd1=fopen("sendfile.txt","rb");
			/*rewind(fd1);
			//fseek(fd1,0,SEEK_END);
			int filesize1=filesize+20+strlen(postdata);
			filesize1=ftell(fd1);
			printf("@@@@@@@@@@@@@@@@@@@@The Filesize: %d\n",filesize1);
			char *buffer1 = (char *)malloc(filesize1);
			bzero(buffer1,filesize1);
			fseek(fd1, 0, SEEK_SET);
			int ny1=fread(buffer1,1,filesize1,fd1);
			printf("@@@@@@@@)))))))))): %d\n",ny1);
			nnbytes=send(fd_client,buffer1,ny1,0);*/
			fclose(fd1);
		
			
		}
		printf("Total bytes sent: %d\n",nnbytes);
		free(buffer);
		
		free(sendHeader);
		fclose(fd);
		close(fd_client);
		}
	}

}
int checkDetails(char *cData, char *check, int len){
	FILE *f,*f1;int flag=0;
	f = fopen("check.txt","w+");
	if(f==NULL){
		perror("File not created");
	}
	fputs(cData,f);
	fclose(f);
	//fseek(f,0,SEEK_END);
	f1 = fopen("check.txt","r");
	fseek(f1,0,SEEK_END);
	int size=ftell(f1);
	fseek(f1,0,SEEK_SET);
	//printf("%d",size);
	char *getCont=(char *)malloc(MAXSIZE);	
	
	while(fgets(getCont,size,f1)!=NULL){
		//printf("Main:%s\n",getCont);
		if(!strncmp(getCont,check,len)){
			return 1;
			flag=1;
			break;
		}
	}
	if(flag==0)
		return 0;
	
}
void checkForFile(int client){
	char *data=(char*)malloc(MAXSIZE);
	char *method=(char*)malloc(MAXSIZE);
	char *filename=(char*)malloc(MAXSIZE);
	char *version=(char*)malloc(MAXSIZE);
	char *connection=(char*)malloc(MAXSIZE);
	char *header = (char*)malloc(MAXSIZE);
	//char *file;
	int i=0,j=4;
	bzero(data,sizeof(data));
	bzero(method,sizeof(method));
	bzero(filename,sizeof(filename));
	bzero(version,sizeof(version));
	bzero(connection,sizeof(connection));
	bzero(header,sizeof(header));
	int connect=0;int get=0;int n;
	/*n=recv(client,data,MAXSIZE,0);
	printf("%s\n",data);
	//printf("%d",strlen("Connection: keep-alive"));
	connect=checkDetails(data,"Connection: keep-alive",strlen("Connection: keep-alive"));
	get=checkDetails(data,"GET",3);
	printf("Flagggg%d",get);
	int i=0,j=4,q;
	//for(q=0;q<9;q++)
	//	printf("%s\n",fileTypes[q]);
	//printf("%s",defaultFile);*/
	while(1){
		printf("InsideL: %d\n",getpid());
		bzero(data,MAXSIZE);
		n=recv(client,data,MAXSIZE,0);
		if(n<1){
			//printf("################### %d\n",n);
			close(client);
			exit(0);
		}
		char* tempstr = calloc(strlen(data)+1, sizeof(char));
		strcpy(tempstr, data);
		printf("%s\n",data);
		method=strtok(tempstr, " \t\n");
		mainMethod=calloc(strlen(method)+1,sizeof(char));
		strcpy(mainMethod,method);
		//printf("@@@@@@@@@@@@@@%s\n",mainMethod);
		//printf("%s\n",method);
		filename=strtok(NULL, " \t");
		//printf("%s\n",filename);
		version=strtok(NULL, " \t\n");
		//printf("%s\n",data);
		if(strncmp(method,"GET",3)==0||strncmp(method,"POST",4)==0||strncmp(method,"DELETE",6)==0||strncmp(method,"PUT",3)==0||strncmp(method,"OPTIONS",7)==0||strncmp(method,"PATCH",4)==0||strncmp(method,"TRACE",5)==0||strncmp(method,"CONNECT",7)==0||strncmp(method,"HEAD",4)==0){
		if(strncmp(method,"GET",3)==0||strncmp(method,"POST",4)==0){
			if(strncmp(method,"POST",4)==0){
				postdata=(char *)malloc(MAXSIZE);
				postdata=strtok(NULL, " \t\n");
				postdata=strtok(NULL, " \t\n");
				postdata=strtok(NULL, " \t\n");
				postdata=strtok(NULL, " \t\n");
				postdata=strtok(NULL, " \t\n");
				postdata=strtok(NULL, " \t\n");
				//printf("&&&&&&&&&&&&&&%s\n",postdata);
			}
		//printf("%s\n",method);
		connect=checkDetails(data,"Connection: keep-alive",strlen("Connection: keep-alive"));
		if(connect==1){
			strcpy(connection,"Connection: keep-alive");
		}
		else{
			//printf("%s\n",method);
			close(client);
			exit(0);
		}
		//printf("%d",strncmp(version,"HTTP/1.1",8));
		if(strncmp(version,"HTTP/1.1",8)==0){
			strcpy(version,"HTTP/1.1");
			//printf("%d",strlen(version));
		}
		//printf("kk%s\n",version);
		//printf("%s\n",connection);
		if(strncmp(version,"HTTP/1.1",8)!=0){
			strncat(header,version,strlen(version));
			strncat(header," 400 Bad Request\n",strlen(" 400 Bad Request\n"));
			strncat(header,"Content-Type:NONE\n",strlen("Content-Type:NONE\n"));
			strncat(header,"Content-Lenght:NONE\n",strlen("Content-Lenght:NONE\n"));
			strncat(header,connection,strlen(connection));
                	strncat(header,"\r\n",strlen("\r\n"));
                	strncat(header,"\r\n",strlen("\r\n"));
                	strncat(header,"<head><title>400 Bad Request Reason</title></head>",strlen("<head><title>400 Bad Request Reason</title></head>"));
                strncat(header,"<html><body>400 Bad Request Reason: Invalid HTTP: ",strlen("<html><body>400 Bad Request Reason: Invalid HTTP: "));
               		strncat(header,version,strlen(version));
                strncat(header,"</BODY></html>",strlen("</BODY></html>"));
                	strncat(header,"\r\n",strlen("\r\n"));
                	//printf("%s\n",header);
			send(client, header, strlen(header)-1,0);
			close(client);
			exit(0);
		}
		//printf("%s",header);
		//printf("%d",strncmp(version,"HTTP/1.1",8));
		else{	
			//printf("HELLLLLLLLOOOOOOOOOOO\n");
			char *filepath=(char*)malloc(MAXSIZE);
			//char *filename=(char*)malloc(MAXSIZE);
			while(data[j]!=' '){
				filepath[i]=data[j];i++;j++;
			}
			//printf("************************************************: %s %d\n",filepath,strlen(filepath));
			if(filepath[2]=='\0'){
				char *file=(char*)malloc(strlen(defaultFile)+12);;
				//memset(file, '\0', sizeof(file));
				strcpy(file,defaultFile);
				//printf("Def: %d\n",strlen(file));
				strcat(file,"/index.html");
				//printf("here");
				//sendFileToBrowser(file,client);
				//printf("fileeeeeeeeeeeeeeeeeeeeee: %s\n",file);
				sendFileToBrowser(file,client,version,connection);
			
			}
			else{
				char *file=(char*)malloc(strlen(defaultFile)+strlen(filepath)+1);;
				//memset(file, '\0', sizeof(file));
				strcpy(file,defaultFile);
				//printf("Def: %d\n",strlen(file));
				strcat(file,filepath);
				//printf("fi: %d\n",strlen(file));
				//printf("%s %s",file,defaultFile);	
				filename=filepath;
				//sendFileToBrowser(file,client);
				//printf("qq%s",filepath);
				//printf("fileeeeeeeeeeeeeeeeeeeeee: %s\n",file);
				sendFileToBrowser(file,client,version,connection);
			}
			//printf("fileeeeeeeeeeeeeeeeeeeeee: %s\n",file);
			//sendFileToBrowser(file,client,version,connection);
	
		}
		
		}
		else{
		strncat(header,version,strlen(version));
			strncat(header," 501 Not Implemented\n",strlen(" 501 Not Implemented\n"));
			strncat(header,"Content-Type:NONE\n",strlen("Content-Type:NONE\n"));
			strncat(header,"Content-Lenght:NONE\n",strlen("Content-Lenght:NONE\n"));
			strncat(header,connection,strlen(connection));
                	strncat(header,"\r\n",strlen("\r\n"));
                	strncat(header,"\r\n",strlen("\r\n"));
                	strncat(header,"<head><title>501 Not Implemented</title></head>",strlen("<head><title>501 Not Implemented</title></head>"));
                strncat(header,"<html><body>501 Not Implemented: ",strlen("<html><body>501 Not Implemented: "));
               		strncat(header,method,strlen(method));
                strncat(header,"</BODY></html>",strlen("</BODY></html>"));
                	strncat(header,"\r\n",strlen("\r\n"));
                	//printf("%s\n",header);
			send(client, header, strlen(header)-1,0);
			close(client);
			exit(0);
		
	}
	}
	else{
		strncat(header,version,strlen(version));
			strncat(header," 400 Bad Request\n",strlen(" 400 Bad Request\n"));
			strncat(header,"Content-Type:NONE\n",strlen("Content-Type:NONE\n"));
			strncat(header,"Content-Lenght:NONE\n",strlen("Content-Lenght:NONE\n"));
			strncat(header,connection,strlen(connection));
                	strncat(header,"\r\n",strlen("\r\n"));
                	strncat(header,"\r\n",strlen("\r\n"));
                	strncat(header,"<head><title>400 Bad Request</title></head>",strlen("<head><title>400 Bad Request</title></head>"));
                strncat(header,"<html><body>400 Bad Request Reason: Invalid Method: ",strlen("<html><body>400 Bad Request Reason: Invalid Method: "));
               		strncat(header,method,strlen(method));
                strncat(header,"</BODY></html>",strlen("</BODY></html>"));
                	strncat(header,"\r\n",strlen("\r\n"));
                	//printf("%s\n",header);
			send(client, header, strlen(header)-1,0);
			close(client);
			exit(0);
		
	}
	}
	

	/*char *filepath=(char*)malloc(MAXSIZE);
	char *filename=(char*)malloc(MAXSIZE);
	while(data[j]!=' '){
		filepath[i]=data[j];i++;j++;
	}
	//printf("************************************************%s %d\n",filepath,strlen(filepath));
	if(filepath[2]=='\0'){
		char *file=(char*)malloc(strlen(defaultFile)+12);;
		//memset(file, '\0', sizeof(file));
		strcpy(file,defaultFile);
		//printf("Def: %d\n",strlen(file));
		strcat(file,"/index.html");
		//printf("here");
		sendFileToBrowser(file,client);
		return filepath;
	}
	else{
		char *file=(char*)malloc(strlen(defaultFile)+strlen(filepath)+1);;
		//memset(file, '\0', sizeof(file));
		strcpy(file,defaultFile);
		//printf("Def: %d\n",strlen(file));
		strcat(file,filepath);
		//printf("fi: %d\n",strlen(file));
		//printf("%s %s",file,defaultFile);	
		filename=filepath;
		sendFileToBrowser(file,client);
		//printf("qq%s",filepath);
		return filepath;
	}
	
	return filepath;*/
	
}	
int main(int argc, char *argv[]){
	struct sockaddr_in client_addr, server_addr;
	socklen_t sin_len = sizeof(client_addr);
	int fd_client, filesize;
	char buf[2048];
	int fdimg,i;
	int on = 1;
	//FILE *fd;
	
	readWS();
	
		
	fd_server = socket(AF_INET, SOCK_STREAM,0);
	
	if(fd_server < 0){
		perror("Socket");
		exit(1);
	}
	
	//setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT80);
	
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
	
	/*fd=fopen("index.html","rb");
	if(fd<0){
		perror("File Not Found");
	}
	fseek(fd,0,SEEK_END);
	filesize=ftell(fd);
	rewind(fd);
	
	char *buffer = (char *)malloc(filesize);
	char *bufff = (char *)malloc(filesize);
	
	fread(buffer,filesize,1,fd);
	printf("%d",strlen(buffer));*/
	char *bufff = (char *)malloc(filesize);
	while(1){
		fd_client = accept(fd_server, (struct sockaddr*) &client_addr, &sin_len);
		printf("MainL: %d %d\n",getpid(),fd_server);
		if(fd_client == -1){
			perror("Accept");
			continue;
		}
		printf("Connected to Client \n");
		
		if(!fork()){
			//close(fd_server);
			
			checkForFile(fd_client);
			
			//printf("ww%s\n",bu);
			//nbytes=send(fd_client,webpage,sizeof(webpage)-1,0);
			//nnbytes=send(fd_client,buffer,strlen(buffer)-1,0);
			
			
			//printf("%d %d",nbytes,nnbytes);
			close(fd_client);
			exit(0);
		}
		close(fd_client);
		
	}
	close(fd_server);
	
	
	return 0;
}
