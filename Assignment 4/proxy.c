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
#include <openssl/md5.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>

#define SIZE 500
#define MAXSIZE 9999

int PORT;

int exists(const char *fname){
    FILE *file;
    if (file = fopen(fname, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}

char* getMD5(char *checksite){
	//printf("Hello in CheckFile: %s\n",checksite);
	char* temp=(char*)malloc(MAXSIZE);
	char* file=(char*)malloc(MAXSIZE);
	char md5sum[100];int i;
	
	bzero(md5sum, sizeof(md5sum));
	
	MD5_CTX mdContext;
       	MD5_Init(&mdContext);
        MD5_Update (&mdContext, checksite, strlen(checksite));
       	MD5_Final (md5sum, &mdContext);

	for (i = 0; i< MD5_DIGEST_LENGTH; i++)
            {
                sprintf(&temp[2*i],"%02x", md5sum[i]);
            }

	sprintf(file,"%s.txt",temp);
        //printf("md5sum %s\nFilename: %s\n", temp,file);
            
	return file;
}
int checkExpiration(char *filepath, char* timer){
	struct stat s;int i,j=0;
	stat(filepath, &s);
    	char timew[MAXSIZE];
    	char temptime[20];int h,m,ss,h1,m1,ss1;
    	sprintf(timew,"%s",ctime(&s.st_mtime));
    	//printf("File Creation time: %s\n",timew);
    	
    	int len=strlen(timew);
    	int stlen=len-14;
    	for (i=stlen;i<len;i++){
    		temptime[j]=timew[i];j++;
    	}
    	h=atoi(strtok(temptime,":"));
    	m=atoi(strtok(NULL,":"));
    	ss=atoi(strtok(NULL," "));
    	
    	//printf("Time: %d %d %d\n",h,m,ss);
    	
    	
    	time_t ctimer;
        time(&ctimer);
        bzero(timew,sizeof(timew));
        sprintf(timew,"%s",ctime(&ctimer));
        //printf("Current Time: %s\n", timew);
        len=strlen(timew);
    	stlen=len-14;
    	j=0;
    	for (i=stlen;i<len;i++){
    		temptime[j]=timew[i];j++;
    	}
    		
    	h1=atoi(strtok(temptime,":"));
    	m1=atoi(strtok(NULL,":"));
    	ss1=atoi(strtok(NULL," "));
    	
    	//printf("Time: %d %d %d\n",h1,m1,ss1);
    	m=m*60+ss;
    	m1=m1*60+ss1;
    	int checktime = atoi(timer);
    	if(h==h1&&m1-m<checktime){
    		return 1;
    	}
    	else{
    		//printf("TIME OUT! I'm Deleting %s from cache!\n",filepath);
    		char* deletecommand=(char*)malloc(strlen(filepath)+4);
    		sprintf(deletecommand,"rm %s",filepath);
    		system(deletecommand);
   		return 0;
    	}
        
}
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
char checkCache(int client, char* time, char* dir, char* method,char* path,char*http_ver,char* site,char*url,char*web){
	struct hostent *connectWebsite;
	struct sockaddr_in host;
	char* location = (char*)malloc(MAXSIZE);
	char*messageToHost=(char*)malloc(MAXSIZE);
	int len,fd_server,on=1;int connection;
	char* file=getMD5(web);
	int bytes,flag;
	//printf("%s\n",file);
	
	sprintf(location,"%s%s",dir,file);
	
	//printf("Location: %s\n",location);
	
	flag=checkExpiration(location,time);
	
	if(flag){
		printf("Sending from Cache!\n");
		FILE *fd;
		fd=fopen(location,"r");
		if(fd==NULL){
			//perror("Blak\n");
		}
		bzero(messageToHost,sizeof(messageToHost));
		do{	
			//printf("Upar!!\n");
			bytes=fread(messageToHost,1,MAXSIZE,fd);
			//printf("Upar2: %d!!\n",bytes);
			bytes=send(client,messageToHost,bytes,0);
			//printf("Upar3: %d!!\n",bytes);
		
		}while(bytes>0);
		
		fclose(fd);
		close(client);
	}
	else{
		
		
		FILE *f1;int q=0;
		f1=fopen("host","ab+");
		
		fseek(f1,0,SEEK_END);
		int fs=ftell(f1);
		fseek(f1,0,SEEK_SET);
		char *data=(char *)malloc(100);
		char *ip_addr=(char *)malloc(100);
		unsigned long inaddr;
		while(fgets(data,fs,f1)!=NULL){
			if(!strncmp(data,site,strlen(site))){
				//printf("%s\n",data);
				ip_addr=strstr(data," ");
				q=1;
				break;
			}
		
		}
		
		if(q){
			ip_addr=trimwhitespace(ip_addr);
			//printf("Here in IF%s\n",ip_addr);
			bzero(&host,sizeof(host));
		         
       			host.sin_family = AF_INET;           
       			host.sin_port = htons(80); 
       			inaddr = inet_addr(ip_addr);
       			//printf("Here in IF%lu\n",inaddr);
       			memcpy(&host.sin_addr, &inaddr, sizeof(inaddr));q=0;
		}
		
		else{
			connectWebsite = gethostbyname(site);
			if (!connectWebsite){
           			perror("Inavlid host address");
           			exit(1);
			}
		
			bzero(&host,sizeof(host));
		         
       			host.sin_family = AF_INET;           
       	 		host.sin_port = htons(80); 
        		bcopy((char*)connectWebsite->h_addr,(char*)&host.sin_addr.s_addr,connectWebsite->h_length); 
        		sprintf(messageToHost,"%s %s\n",site,inet_ntoa(host.sin_addr));
        	
        		//printf("Here in else%s\n",messageToHost);
        
        		fwrite(messageToHost,1,strlen(messageToHost),f1);
        	
		}
		//bzero(ip_addr,sizeof(ip_addr));
		fclose(f1);
		 
        	sprintf(messageToHost,"\nConnecting to %s  IP - %s\n",site,inet_ntoa(host.sin_addr));
        	
        	//printf("%s\n",messageToHost);
        	len = sizeof(host);
       		fd_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        	if (fd_server<0){
               		perror("HOST socket creation failed");
        	}
        	
        	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, 4); 

       		connection = connect(fd_server, (struct sockaddr *) &host, len);
        	if (connection < 0){
           		printf("Connection problem\n");
           		close(fd_server);
      		}
		//printf("%s\n",inet_ntoa(host.sin_addr));
		
		if(path!=NULL)
			sprintf(messageToHost,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",url,http_ver,site);
		else
			sprintf(messageToHost,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",http_ver,site);

		//printf("messageToHost %s\n", messageToHost);
		
		int nbytes = send(fd_server,messageToHost,strlen(messageToHost),0);
        	printf("Storing New data into the Cache\n");
		if (nbytes < 0){
        		//perror("Host send failed");
        	}
        	
        	else{	
        		FILE *fd;
			fd=fopen(location,"ab");
        		char* data=(char*)malloc(MAXSIZE);
        	      	do{          
        	                bzero((char*)data,MAXSIZE);
        	               	//printf("Upar\n");
        	                nbytes=read(fd_server,data,MAXSIZE);       
        	                //printf("Neeche %d\n",nbytes);
        	                if(nbytes<=0){
        	                	//perror("Unable to Recv!\n");
        	                }
        	                
        	                else{
        	                    send(client,data,nbytes,0);                                  
        	                    fwrite(data,1,nbytes,fd);
        	                }
        	                
        	        }while(nbytes>0);
        	        fclose(fd);
		}
		
		close(fd_server);
		close(client);
	}
	
	
	
}
int checkBlockList(char* web, char* IP){
	//printf("BlockList : %s %s\n",web,IP);
	FILE *f;
	f=fopen("blocklist","r");
	int filesize,fl=0;
	fseek(f,0,SEEK_END);
	filesize=ftell(f);
	fseek(f,0,SEEK_SET);
	char *fileContent=(char *)malloc(100);
	while(fgets(fileContent,filesize,f)!=NULL){
		//printf("%s\n",fileContent);
		if(!strncmp(web,fileContent,strlen(web))||!strncmp(IP,fileContent,strlen(IP))){
			fl=1;
			break;
		}
	}
	if(fl)
		return 0;
	else
		return 1;
}
int checkForBlock(char*webs){
	int r=0;
	//printf("Checking for Block!\n");
	struct hostent *connectWebsite;
	struct sockaddr_in host;
	char*messageToHost=(char*)malloc(MAXSIZE);
	connectWebsite = gethostbyname(webs);
	if (!connectWebsite){
           	perror("Inavlid host address");
           	exit(1);
	}
		
	bzero(&host,sizeof(host));
		         
       	host.sin_family = AF_INET;           
       	host.sin_port = htons(80); 
        bcopy((char*)connectWebsite->h_addr,(char*)&host.sin_addr.s_addr,connectWebsite->h_length);  
        	  
        sprintf(messageToHost,"\nConnecting to %s  IP - %s\n",webs,inet_ntoa(host.sin_addr));
        	
        //printf("%s\n",messageToHost);
        
        r=checkBlockList(webs,inet_ntoa(host.sin_addr));
        
        return r;
        	
}
void respond(int client, char* time, char* dir){

	//printf("Into Respond: %d %s %s\n",client, time, dir);
	
	char messageFrmClient[MAXSIZE];
	char method[MAXSIZE];
    	char path[MAXSIZE];
	char http_ver[MAXSIZE];
	char *web=(char*)malloc(MAXSIZE);
	char *site=(char*)malloc(MAXSIZE);
	char *url=(char*)malloc(MAXSIZE);
	
	bzero(web,sizeof(web));
	int i;int flag=0;int k=0;
	
	
	bzero(messageFrmClient, sizeof(messageFrmClient));
	
	if (recv(client, messageFrmClient, MAXSIZE,0)<0){
        	printf("recieve error\n");
    	}
    	//printf("Message from Server:\n %s\n",messageFrmClient);
    	sscanf(messageFrmClient,"%s %s %s",method,path,http_ver);
	//printf("messageFrmClient: %s %s %s\n",method,path,http_ver);
	
	if(((strncmp(method,"GET",3)==0))&&((strncmp(http_ver,"HTTP/1.1",8)==0)||(strncmp(http_ver,"HTTP/1.0",8)==0))&&(strncmp(path,"http://",7)==0)){
	//printf("%s\n",path);
		char *mainPath=(char*)malloc(MAXSIZE);
		strcpy(mainPath,path);
		web = strstr(path,"//");
		int f=0;
	//printf("%s\n",web);
	
		for (i=0;i<strlen(web);i++)
			web[i]=web[i+2];

		//printf("%s\n",web);
	
		for(i=0;i<strlen(web);i++){
			if(flag==0){
				if (web[i]!='/')
					site[i]=web[i];
				else
					flag=1;
			}
			else{
				url[k]=web[i];k++;
			}
		}
		
		f = checkForBlock(site);
		if(f){
			checkCache(client,time,dir,method,path,http_ver,site,url,web);
		}
		else{
			write(client,"<html><body><H1>Error 403 Forbidden </H1></body></html>",strlen("<html><body><H1>Error 403 Forbidden </H1></body></html>"));
		}
		//close(client);
	}
		
	else{
		if(strncmp(method,"GET",strlen("GET"))){
      			write(client,"<html><body><H!>Error 400 Bad Request: Invalid Method </H1></body></html>",strlen("<html><body><H!>Error 400 Bad Request: Invalid Method </H1></body></html>"));
      				 
    		}
    
   		 else if(strncmp(http_ver,"HTTP/1.0",strlen("HTTP/1.0"))){
      			write(client,"<html><body><H1>Error 400 Bad Request: Invalid HTTP Version</H1></body></html>",strlen("<html><body><H1>Error 400 Bad Request: Invalid HTTP Version</H1></body></html>"));
      				          
		}
		
		//printf("Will not support this\n");
		close(client);
		return;
	}
		
}

int main(int argc, char* argv[]){
	
	char timeout[10];
	char maindir[SIZE];
	char cachedir[SIZE];
	
	char pwd[SIZE];
	
	if (argc<3)
    	{
        	printf("Invalid argumants!\n");
        	exit(1);
	}
	
	PORT=atoi(argv[1]);
	strcpy(timeout,argv[2]);
	
	//printf("PORT number: %d\n", PORT);
	//printf("Cache Timeout is: %s\n", timeout);
	
	struct sockaddr_in client_addr, server_addr;
	socklen_t sin_len = sizeof(client_addr);
	int fd_server, fd_client,on=1;
	
	fd_server = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	
	if(fd_server < 0){
		perror("Socket");
		exit(1);
	}
	
	
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	
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
	
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	//setsockopt(fd_server, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(int));
	
	if (getcwd(maindir, sizeof(maindir)) != NULL){
        	//printf("Current working dir: %s\n", maindir);
        	sprintf(pwd,"%s/cache/",maindir);
        	sprintf(cachedir,"mkdir -p %s",pwd);
        	system(cachedir);
    	}
	//printf("Current working Directory: %s\n",pwd);
	char message[MAXSIZE];
	while(1){
		bzero(message,sizeof(message));
		fd_client = accept(fd_server, (struct sockaddr*) &client_addr, &sin_len);
		
		if(fd_client == -1){
			perror("Accept");
			continue;
		}
		//printf("Connected to Client \n");
		
		if(!fork()){
			//printf("Up\n");
			respond(fd_client, timeout, pwd);
			//read(fd_client, message, MAXSIZE);
			//printf("Here: %s\n",message);
			close(fd_client);
			
		}
		else{
			close(fd_client);
			exit(0);
		}
		
	}
	close(fd_server);

	return 0;
}
