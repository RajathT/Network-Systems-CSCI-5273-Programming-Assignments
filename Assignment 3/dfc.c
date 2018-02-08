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
#include<time.h>

#define TIMEOUT 1
#define MAX 4
#define MAXFILE 999999

int PORTS[4];
char* SERVERS[4];
int serverCount;
int sockets[MAX];
char *username;
char *password;
int activeServers[4];


void readConf(char *conf){
	FILE *fd;int i=0,count=0;
	fd = fopen(conf,"r");
	fseek(fd,0,SEEK_END);
	int size=ftell(fd);
	char *fileContent=(char *)malloc(100);
	rewind(fd);
	while(fgets(fileContent,size,fd)!=NULL){
		//printf("%s",fileContent);
		//printf("U:%s P:%s\n",username,password);
		if (strncmp(fileContent, "Server", 6)==0){
			strtok(fileContent," ");
			strtok(NULL, " ");
			fileContent=strtok(NULL, " ");
			SERVERS[i]=(char*)malloc(100);
			SERVERS[i]=fileContent;
			fileContent=strtok(NULL, " ");
			//printf("File: %s\n",fileContent);
			PORTS[i]=atoi(fileContent);i++;count++;
		}
		else if(strncmp(fileContent, "Username", 8)==0){
			//printf("%s\n",fileContent);
			strtok(fileContent," ");
			//printf("%s\n",fileContent);
			fileContent=strtok(NULL, " \t");
			//printf("%s\n",fileContent);
			username=(char*)malloc(100);
			strcpy(username,fileContent);
		}
		else if(strncmp(fileContent, "Password", 8)==0){
			//printf("%s\n",fileContent);
			strtok(fileContent," ");
			//printf("%s\n",fileContent);
			fileContent=strtok(NULL, " \t");
			//printf("%s\n",fileContent);
			password=(char*)malloc(100);
			strcpy(password,fileContent);
			
		}
		
		bzero(fileContent,sizeof(fileContent));
	}
	//printf("%s %s\n",username,password);
	serverCount=count;
}


int createSockets(int ch){
	int i,myServerlen;
	struct hostent *myServer;
	struct sockaddr_in myServer_addr[serverCount];
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
	if(ch==-1){
	for(i=0;i<serverCount;i++){
		if((sockets[i]=socket(AF_INET, SOCK_STREAM, 0))==-1){
			perror("Socket Failed to Create");
			//exit(0);
		}
		//printf("%s %d\n",SERVERS[i],sockets[i]);
		if(setsockopt(sockets[i],SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout))<0){
			perror("Socket Operation Failed\n");
		}
		myServer=gethostbyname(SERVERS[i]);
		
		bzero((char *) &myServer_addr[i], sizeof(myServer_addr[i]));
		memset((char *)&myServer_addr[i],0,sizeof(myServer_addr[i]));
		
		myServer_addr[i].sin_family = AF_INET;
		bcopy((char *)myServer->h_addr, (char *)&myServer_addr[i].sin_addr.s_addr, myServer->h_length);
		//printf("%d\n",PORTS[i]);
		myServer_addr[i].sin_port = htons(PORTS[i]);
		myServerlen = sizeof(myServer_addr[i]);
	
		if(connect(sockets[i],(struct sockaddr*)&myServer_addr[i],myServerlen)<0){
			perror("Couldn't Connect");
		}
		
	}
	return 1;
	}
	else{
		myServer=gethostbyname(SERVERS[ch]);
		
		bzero((char *) &myServer_addr[ch], sizeof(myServer_addr[ch]));
		memset((char *)&myServer_addr[ch],0,sizeof(myServer_addr[ch]));
		
		myServer_addr[ch].sin_family = AF_INET;
		bcopy((char *)myServer->h_addr, (char *)&myServer_addr[ch].sin_addr.s_addr, myServer->h_length);
		//printf("%d\n",PORTS[i]);
		myServer_addr[ch].sin_port = htons(PORTS[ch]);
		myServerlen = sizeof(myServer_addr[ch]);
	
		if(connect(sockets[ch],(struct sockaddr*)&myServer_addr[ch],myServerlen)<0){
			perror("Couldn't Connect");
			return -1;
		}
		else{
			return ch;
		}
	}
}

int calculateMD5(char* path,char *file){
	//printf("%s\n",file);
	//file = (char*)malloc(200);
	FILE *fd;
	////char *filename = (char*)malloc(200);
	////bzero(filename,sizeof(filename));
	//printf("File: %s\n",filename);
	//filename=getenv("PWD");
	////strcpy(filename,path);
	//printf("File1: %s\n",filename);
	////strcat(filename,"/Blah/");
	////strcat(filename,file);
	//printf("File1: %s\n",filename);
	fd = fopen(path,"r");
	//printf("%d\n",fd);
	if(fd==NULL){
		perror("File Not Found");
		return -1;
	}
	else{
		fseek(fd,0,SEEK_END);
		int filesize = ftell(fd);
		rewind(fd);
		//printf("%d\n",filesize);
		char * data = (char*) malloc(filesize);
		int bytesRead = fread(data, 1, filesize, fd);
		//printf("%d\n",bytesRead);
	//MD5 HASHING
		char *command = (char*)malloc(200);
		strcpy(command,"md5sum ");
		strcat(command,path);
		strcat(command,">file.txt");
		//printf("%s\n",command);
		system(command);
		FILE *fd1;
		fd1=fopen("file.txt","r");
		fseek(fd1,0,SEEK_END);
		int filesize1 = ftell(fd1);
		rewind(fd1);
		char *readbuf=(char*)malloc(200);
		char* md5hash=(char*)malloc(200);
		while(fgets(readbuf,filesize,fd1)!=NULL){
		//printf("%s\n",readbuf);
			md5hash=strtok(readbuf," \t\n");
			break;
		}
	
		//printf("YUP: %s\n",md5hash);
		int v1, v2, v3, v4;
		sscanf( &md5hash[0], "%x", &v1 );
		sscanf( &md5hash[8], "%x", &v2 );
		sscanf( &md5hash[16], "%x", &v3 );
		sscanf( &md5hash[24], "%x", &v4 );
		//printf("%d %d %d %d\n",v1,v2,v3,v4);
		if(v4<0)
			v4=v4*-1;
		//printf("%d",v4%4);
		return v4%4;
		fclose(fd1);
	}
	fclose(fd);
	
	
}
char* operations(){
	char* command=(char*)malloc(100);
	char* filename=(char*)malloc(100);
	char* subFolder=(char*)malloc(100);
	printf("Choose your option by typing [command] [filename] [subfolder]. If no subfolder type NULL\n");
	printf("Operations supported:\n1.list\n2.get\n3.put\n");
	scanf("%s",command);
	scanf("%s",filename);
	scanf("%s",subFolder);
	strcat(command, " ");
	strcat(command, filename);
	strcat(command, " ");
	strcat(command, subFolder);
	//printf("%s\n",command);
	return command;
}
int formConnection(char *user1,char *pass1){
	int i,bytes=-1,flag=0;
	char *user=(char*)malloc(100);
	strcpy(user,user1);
	char *pass=(char*)malloc(100);
	strcpy(pass,pass1);
	strcat(user, " ");
	strcat(user, pass);
	printf("Authentication via sending Credentials...\n");
	//printf("%s %d %s %d\n",user,serverCount,pass,strlen(user));
	//send(sockets[1],user,strlen(user),0);
	for (i=0;i<serverCount;i++){
		//printf("For loop: %d %d\n",i,sockets[i]);
		printf("DATATA SENT:%s\n",user);
		bytes=send(sockets[i],user,strlen(user),MSG_NOSIGNAL);
		
		if(bytes<=0){
			printf("Socket not available is: %d\n",sockets[i]);
			activeServers[i]=0;
			continue;
		}
		else{
			printf("Socket available is: %d\n",sockets[i]);
			
			char *data=(char*)malloc(2048);
			int n=recv(sockets[i],data,2048,0);
			printf("Message from servers: %s\n",data);
			if(n>0)
				activeServers[i]=1;
			if(data[4]=='S')
				flag=1;
		}
	}
	
	return flag;
}
char * xorencrypt(char * message, char * key,int len) {
    size_t keylen = strlen(key);int i;
    char * encrypted = malloc(len);

    for(i = 0; i < len; i++) {
        encrypted[i] = message[i] ^ key[i % keylen];
    }

    return encrypted;
}
void putFileToServer(char* pather,char *filename,char*subfold){
	//printf("!!!!!!!!!!!%s\n",pather);
	//printf("File to send is %s to subfolder %s\n",filename,subfold);
	char *path = (char*)malloc(200);int i;
	int signal=0,flag=0;
	//path=getenv("PWD");
	strcpy(path,pather);
	strcat(path,"/Blah/");
	strcat(path,filename);
	//printf("QQQQQQ: %s\n",path);
	int mdValue=calculateMD5(path,filename);
	printf("MDValue of %s is %d\n",filename,mdValue);
	//printf("%s %s\n",username,password);
	//printf("Forming connections with the Server..\n");
	signal=formConnection(username,password);
	//printf("!!!!!!!!!!!!!!!!I'm Here! %d\n",signal);
	for(i=0;i<serverCount;i++){
		if(activeServers[i])
			flag=1;
		//printf("%d\n",activeServers[i]);
	}
	//No Active Servers
	//flag=1;
	//signal=1;
	if(flag){
	if(signal){
		//printf("I can send now\n");
		FILE *fd;
		fd=fopen(path,"r");
		if(fd==NULL){
			printf("File Not Found\n");
			
			return;
		}
		
		fseek(fd,0,SEEK_END);
		int filesize=ftell(fd);
		rewind(fd);
		//printf("Hello@@@@@@@@@@@@@@%d \n",filesize);
		char *fileData=(char*)malloc(filesize);
		int bytes=fread(fileData,filesize,1,fd);
		//printf("%d %d\n",bytes,filesize);
		int eachFile=filesize/4;
		int remains=filesize%4;
		//printf("Each: %d %d\n",eachFile,remains);
		char *part1 = (char*)malloc(eachFile);
		int j=0;
		//printf("0 %d\n",eachFile-1);
		int filesize1=0,filesize2=0, filesize3=0, filesize4=0;
		for(i=0;i<eachFile;i++){
			part1[j]=fileData[i];j++;
			filesize1++;
		}
		part1=xorencrypt(part1, password,filesize1);
		int start=eachFile;
		int end=eachFile*2;
		//printf("%d %d\n",start,end);
		char *part2 = (char*)malloc(eachFile);
		j=0;
		for(i=start;i<end;i++){
			part2[j]=fileData[i];j++;
			filesize2++;
		}
		part2=xorencrypt(part2, password,filesize2);
		start=end;
		end=eachFile*3;
		//printf("%d %d\n",start,end);
		char *part3 = (char*)malloc(eachFile);
		j=0;
		for(i=start;i<end;i++){
			part3[j]=fileData[i];j++;
			filesize3++;
		}
		part3=xorencrypt(part3, password,filesize3);
		start=end;
		end=eachFile*4+remains;
		j=0;
		//printf("%d %d\n",start,end);
		char *part4 = (char*)malloc(eachFile+remains);
		for(i=start;i<end;i++){
			part4[j]=fileData[i];j++;
			filesize4++;
		}
		part4=xorencrypt(part4, password,filesize4);
		printf("File broken into %d %d %d %d\n",filesize1,filesize2,filesize3,filesize4);
		char*charfilesize1=(char*)malloc(10);
		char*charfilesize2=(char*)malloc(10);
		char*charfilesize3=(char*)malloc(10);
		char*charfilesize4=(char*)malloc(10);
		sprintf(charfilesize1,"%d",filesize1);
		sprintf(charfilesize2,"%d",filesize2);
		sprintf(charfilesize3,"%d",filesize3);
		sprintf(charfilesize4,"%d",filesize4);
		//printf("Filechars: %s %s %s %s\n",charfilesize1,charfilesize2,charfilesize3,charfilesize4);
		//Creating File Names
		//printf("###################%s\n",filename);
		char *filename1 = (char*)malloc(50);
		sprintf(filename1,".%s.%d",filename,1);
		printf("%s\n",filename1);
		
		char *filename2 = (char*)malloc(50);
		sprintf(filename2,".%s.%d",filename,2);
		printf("%s\n",filename2);
		
		char *filename3 = (char*)malloc(50);
		sprintf(filename3,".%s.%d",filename,3);
		printf("%s\n",filename3);
		
		char *filename4 = (char*)malloc(50);
		sprintf(filename4,".%s.%d",filename,4);
		printf("%s\n",filename4);
		
		strcat(filename1, " ");
		strcat(filename1, charfilesize1);
		strcat(filename1, " ");
		strcat(filename1, subfold);
		strcat(filename1," ");
		for(i=strlen(filename1);i<50;i++)
			strcat(filename1,"a");
		strcat(filename2, " ");
		strcat(filename2, charfilesize2);
		strcat(filename2, " ");
		strcat(filename2, subfold);
		strcat(filename2," ");
		for(i=strlen(filename2);i<50;i++)
			strcat(filename2,"a");
		strcat(filename3, " ");
		strcat(filename3, charfilesize3);
		strcat(filename3, " ");
		strcat(filename3, subfold);
		strcat(filename3," ");
		for(i=strlen(filename3);i<50;i++)
			strcat(filename3,"a");
		strcat(filename4, " ");
		strcat(filename4, charfilesize4);
		strcat(filename4, " ");
		strcat(filename4, subfold);
		strcat(filename4," ");
		for(i=strlen(filename4);i<50;i++)
			strcat(filename4,"a");
		
		/*printf("Wah alah!! %s\n",filename1);
		printf("%d\n",strlen(filename1));
		
		printf("Wah alah!! %s\n",filename2);
		printf("%d\n",strlen(filename2));
		
		printf("Wah alah!! %s\n",filename3);
		printf("%d\n",strlen(filename3));
		
		printf("Wah alah!! %s\n",filename4);
		printf("%d\n",strlen(filename4));*/
		
		if(mdValue==0){
			//DFS1
			
			bytes=send(sockets[0],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part1,filesize1,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[0],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part2,filesize2,MSG_NOSIGNAL);
			
			//DFS2
			
			bytes=send(sockets[1],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part2,filesize2,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[1],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part3,filesize3,MSG_NOSIGNAL);
			
			//DFS3
			
			bytes=send(sockets[2],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part3,filesize3,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[2],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part4,filesize4,MSG_NOSIGNAL);
			
			//DFS4
			
			bytes=send(sockets[3],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part4,filesize4,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[3],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part1,filesize1,MSG_NOSIGNAL);
			
			
			
		}
		else if(mdValue==1){
			//printf("%d\n",mdValue);
			//DFS1
			
			bytes=send(sockets[0],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part4,filesize4,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[0],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part1,filesize1,MSG_NOSIGNAL);
			
			//DFS2
			
			bytes=send(sockets[1],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part1,filesize1,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[1],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part2,filesize2,MSG_NOSIGNAL);
			
			//DFS3
			
			bytes=send(sockets[2],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part2,filesize2,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[2],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part3,filesize3,MSG_NOSIGNAL);
			
			//DFS4
			
			bytes=send(sockets[3],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part3,filesize3,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[3],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part4,filesize4,MSG_NOSIGNAL);
			
			
		}
		else if(mdValue==2){
			//printf("%d\n",mdValue);
			
			bytes=send(sockets[0],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part3,filesize3,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[0],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part4,filesize4,MSG_NOSIGNAL);
			//DFS2
			
			bytes=send(sockets[1],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part4,filesize4,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[1],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part1,filesize1,MSG_NOSIGNAL);
			//DFS3
			
			bytes=send(sockets[2],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part1,filesize1,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[2],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part2,filesize2,MSG_NOSIGNAL);
			//DFS4
			
			bytes=send(sockets[3],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part2,filesize2,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[3],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part3,filesize3,MSG_NOSIGNAL);
			
		}
		
		else if(mdValue==3){
			//printf("%d\n",mdValue);
			//printf("%d\n",mdValue);
			
			
			bytes=send(sockets[0],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part2,filesize2,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[0],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[0],part3,filesize3,MSG_NOSIGNAL);
			//DFS2
			
			bytes=send(sockets[1],filename3,strlen(filename3),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part3,filesize3,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[1],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[1],part4,filesize4,MSG_NOSIGNAL);
			//DFS3
			
			bytes=send(sockets[2],filename4,strlen(filename4),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part4,filesize4,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[2],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[2],part1,filesize1,MSG_NOSIGNAL);
			//DFS4
			
			bytes=send(sockets[3],filename1,strlen(filename1),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part1,filesize1,MSG_NOSIGNAL);
			//printf("DTATATATATATSENT: %d %d",strlen(charfilesize1),bytes);
			
			bytes=send(sockets[3],filename2,strlen(filename2),MSG_NOSIGNAL);
			//bytes=send(sockets[0],charfilesize1,strlen(charfilesize1),MSG_NOSIGNAL);
			bytes=send(sockets[3],part2,filesize2,MSG_NOSIGNAL);
			
		}
		else{
			//printf("%d\n",mdValue);
			printf("File MD5 cannot realize\n");
		}
		
	}
	else{
		printf("Invalid credentials!\n");
	}
	}
	else{
		printf("No Active Servers!\n");
	}
	//Make Activeservers 0
	for(i=0;i<serverCount;i++){
		if(activeServers[i])
			activeServers[i]=0;
		//printf("%d\n",activeServers[i]);
	}
}
void getFileFromServer(char *pathr, char* filee,char*suub){
	//printf("%s %s %s\n",pathr,filee,suub);
	int flag=0,signal=0,i,j,k,y,byt;
	signal=formConnection(username,password);
	
	for(i=0;i<serverCount;i++){
		if(activeServers[i])
			flag=1;
		//printf("%d\n",activeServers[i]);
	}
	char* filesend=(char*)malloc(20);
	if(flag){
		if(signal){
			//printf("Bring it on!\n");
			strcpy(filesend,filee);
			strcat(filesend, " ");
			strcat(filesend,suub);
			strcat(filesend, " ");
			//printf("%s\n",filee);
			for(k=0;k<4;k++){
				send(sockets[k],filesend,strlen(filesend),MSG_NOSIGNAL);
				char *ft = (char*)malloc(50);
				bzero(ft,sizeof(ft));
				//printf("Going to recience!!!!!\n");
				byt=recv(sockets[k],ft,50,0);
				//printf("%d %s\n",byt,ft);
				if(byt>0){
				char* fn=(char*)malloc(20);
				fn=strtok(ft," ");
				/*i=0;
				while(ft[i]!=' '){
					fn[i]=ft[i];
					i++;
				}*/
				printf("Recieved File: %s\n",fn);
				i=i+1;j=0;
				char* fs=(char*)malloc(10);
				fs=strtok(NULL," ");
				/*while(ft[i]!=' '){
					printf("%c\n",ft[i]);
					fs[j]=ft[i];
					i++;j++;
				}*/
				//printf("%s\n",fs);
				int fisi=atoi(fs);
				//printf("%d\n",fisi);
				FILE *ftr;
				ftr=fopen(fn,"w");
				char * thedata = (char*)malloc(fisi);
				byt=recv(sockets[k],thedata,fisi,0);
				//printf("%s\n",ft);
				
				thedata=xorencrypt(thedata, password,fisi);
				byt=fwrite(thedata,fisi,1,ftr);
				//printf("%d %s\n",byt,ft);
				bzero(thedata,sizeof(thedata));
				fclose(ftr);
				}
				//printf("Second Fileeeeeeeeeeeeeeeeeeeeeeee\n");
				
				//Second File
				char *ft1 = (char*)malloc(50);
				bzero(ft,sizeof(ft));
				//printf("Going to recience!!!!!\n");
				byt=recv(sockets[k],ft1,50,0);
				//printf("%d %s\n",byt,ft1);
				if(byt>0){
				
				char* fn1=(char*)malloc(20);
				bzero(fn1,sizeof(fn1));
				
				fn1=strtok(ft1," ");
				//printf("%s\n",fn1);
				/*i=0;
				while(ft1[i]!=' '){
					fn1[i]=ft1[i];
					i++;
				}*/
				
				printf("Recieved File: %s\n",fn1);
				char* fs1=(char*)malloc(10);
				bzero(fs1,sizeof(fs1));
				
				fs1=strtok(NULL," ");
				
				/*i=i+1;j=0;
				char* fs1=(char*)malloc(10);
				while(ft1[i]!=' '){
					printf("%c\n",ft1[i]);
					fs1[j]=ft1[i];
					i++;j++;
				}*/
				
				//printf("%s\n",fs1);
				int fisi1=atoi(fs1);
				//printf("%d\n",fisi1);
				FILE *ftr1;
				ftr1=fopen(fn1,"w");
				char * thedata1 = (char*)malloc(fisi1);
				bzero(thedata1,sizeof(thedata1));
				byt=recv(sockets[k],thedata1,fisi1,0);
				//printf("%s\n",ft);
				thedata1=xorencrypt(thedata1, password,fisi1);
				byt=fwrite(thedata1,fisi1,1,ftr1);
				//printf("%d %s\n",byt,ft1);
				//bzero(thedata1,sizeof(thedata1));
				fclose(ftr1);
				}
			}
			//Join the files
			//printf("JOINERRRR!!!");
			char* filer = malloc(strlen(filee)-1);
			filer=strtok(filee," ");
			//printf("%s",filer);
			char *filename1 = (char*)malloc(strlen(filee)+2);
			sprintf(filename1,".%s.%d",filee,1);
			//printf("%s\n",filename1);
		
			char *filename2 = (char*)malloc(strlen(filee)+2);
			sprintf(filename2,".%s.%d",filee,2);
			//printf("%s\n",filename2);
		
			char *filename3 = (char*)malloc(strlen(filee)+2);
			sprintf(filename3,".%s.%d",filee,3);
			//printf("%s\n",filename3);
		
			char *filename4 = (char*)malloc(strlen(filee)+2);
			sprintf(filename4,".%s.%d",filee,4);
			//printf("%s\n",filename4);
			
			FILE *yo1,*yo2,*yo3,*yo4;int fie1,fie2,fie3,fie4;
			yo1=fopen(filename1,"r");
			yo2=fopen(filename2,"r");
			yo3=fopen(filename3,"r");
			yo4=fopen(filename4,"r");
			
			if(yo1==NULL||yo2==NULL||yo3==NULL||yo4==NULL){
				printf("Incomplete Files to form the file!\n");
				return;
			}
			else{
				printf("All Files Recieved\n");
				fseek(yo1,0,SEEK_END);
				fseek(yo2,0,SEEK_END);
				fseek(yo3,0,SEEK_END);
				fseek(yo4,0,SEEK_END);
				
				fie1=ftell(yo1);
				fie2=ftell(yo2);
				fie3=ftell(yo3);
				fie4=ftell(yo4);
				
				//printf("%d %d %d %d\n",fie1,fie2,fie3,fie4);
				
				fseek(yo1,0,SEEK_SET);
				fseek(yo2,0,SEEK_SET);
				fseek(yo3,0,SEEK_SET);
				fseek(yo4,0,SEEK_SET);
				
				char *pa1=(char*)malloc(fie1);
				char *pa2=(char*)malloc(fie2);
				char *pa3=(char*)malloc(fie3);
				char *pa4=(char*)malloc(fie4);
				
				fread(pa1,fie1,1,yo1);
				fread(pa2,fie2,1,yo2);
				fread(pa3,fie3,1,yo3);
				fread(pa4,fie4,1,yo4);
				
				FILE *qw;
				qw=fopen(filee,"w");
				char*cd = (char*)malloc(fie1+fie2+fie3+fie4);
				for(i=0;i<fie1;i++){
					cd[i]=pa1[i];
				}j=0;
				for(i=fie1;i<fie2;i++){
					cd[i]=pa2[j];j++;
				}j=0;
				for(i=fie2;i<fie3;i++){
					cd[i]=pa3[j];j++;
				}j=0;
				for(i=fie3;i<fie4;i++){
					cd[i]=pa4[j];j++;
				}
				int tot=fie1+fie2+fie3+fie4;
				fwrite(cd,tot,1,qw);
				fclose(qw);
				fclose(yo1);
				fclose(yo2);
				fclose(yo3);
				fclose(yo4);
			}
		
			
		}
		else{
			printf("Invalid credentials!\n");
		}
	}
	else{
		printf("No Active Servers!\n");
	}
	
}
/*char* innerCheck(char* filen, char* all){
	printf("Filename: %s\n",filen);
	printf("ALL: %s\n",all);
	/*FILE *hh;
	hh=fopen("qwe.txt","w");
	fwrite(all,strlen(all),1,hh);
	//fseek(hh,0,SEEK_SET);
	fclose(hh);
	FILE *bb;
	bb=fopen("qwe.txt","r");
	fseek(bb,0,SEEK_SET);
	char*bis=(char*)malloc(500);
	fread(bis,strlen(all),1,bb);
	printf("BISS: %s\n",bis);
	
	char* checkfile1 = (char*)malloc(20);
	char* chec = (char*)malloc(20);
	checkfile1=strtok(bis," ");
	chec=strtok(NULL," ");
	while(checkfile1!=NULL){
		printf("Outside: %s\n",checkfile1);
		if(!strncmp(filen,checkfile1,strlen(filen)-1)){
			printf("Inside: %s\n",checkfile1);
		}
		checkfile1=strtok(NULL," ");
		
	}
	checkfile1=strtok(bis," ");
	printf("DONEHERE!");
	return chec;
}*/
void getListOfFiles(char*subu){
	int flag=0,signal=0,i,k,dag=0,br;
	signal=formConnection(username,password);
	
	for(i=0;i<serverCount;i++){
		if(activeServers[i])
			flag=1;
		//printf("%d\n",activeServers[i]);
	}
	char *allfiles = (char*)malloc(500);
	if(flag){
		if(signal){
			//printf("Ola! Let the list begin!\n");
			strcat(subu, " ");
			for(k=0;k<4;k++){
				char* content = (char*)malloc(200);
				send(sockets[k],subu,strlen(subu),MSG_NOSIGNAL);
				send(sockets[k],"Hello",strlen("Hello"),MSG_NOSIGNAL);
				br=recv(sockets[k],content,200,0);
				if(br>5){
				if(dag==0){
					strcpy(allfiles,content);
					dag=1;
				}
				else{
					strcat(allfiles," ");
					strcat(allfiles,content);
				}
				//printf("%s\n",content);
				}
			}
			//printf("Final:\n");
			//printf("%s\n",allfiles);
			char* allcpy = (char*)malloc(500);
			strcpy(allcpy,allfiles);
			//printf("%s\n",allcpy);
			char* checkfile = (char*)malloc(20);
			char* che = (char*)malloc(20);
			char* filearr[100];
			char* filearr1[100];
			checkfile=strtok(allcpy," ");
			int i=0,r=0,l=0;
			while(checkfile!=NULL){
				filearr[i]=(char*)malloc(50);
				filearr1[i]=(char*)malloc(50);
				strcpy(filearr[i],checkfile);
				strcpy(filearr1[i],checkfile);
				i++;
				checkfile=strtok(NULL," ");
			}
			char* filearr2[100];int fl=0;
			//filearr2[0]=filearr[0];
			char* cheq = (char*)malloc(20);int tota[5],u,x;
			for(k=0;k<i;k++){
				//printf("Outside-%s %d\n",filearr[k],strlen(filearr[k]));
			}
			for(k=0;k<i;k++){
				//printf("Outside:%s\n",filearr[k]);
				bzero(cheq,20);
				int rr=strlen(filearr[k])-1;
				for(l=0;l<rr;l++){
					//printf("%c\n",filearr[k][l]);
					cheq[l]=filearr[k][l];
				}
				//printf("CHEQ: %s %d\n",cheq,rr);
				for(u=1;u<=4;u++){
					sprintf(che,"%s%d",cheq,u);
					//printf("NUMCHEQ:%s\n",che);
					for(r=0;r<i;r++){
						//printf("FIle ARR: %s\n",filearr1[r]);
						if(!strcmp(che,filearr[r])){
							//printf("Hey:%s\n",che);
							tota[u]=1;
							strcpy(filearr[r],"a");
							//break;
						}
					}
				}
				//for(x=0;x<i;x++){
				//	printf("EE=%s\n",filearr[x]);
				//}
				if(tota[1]==1&&tota[2]==1&&tota[3]==1&&tota[4]==1){
					char* pri = (char*)malloc(strlen(cheq)-2);int e,h=0;
					for(e=1;e<strlen(cheq)-1;e++){
						pri[h]=cheq[e];h++;
					}
					printf("%s [complete]\n",pri);
					tota[1]=0;tota[2]=0;tota[3]=0;tota[4]=0;
				}
				else{	
					if(strlen(cheq)>3){
						char* pri = (char*)malloc(strlen(cheq)-2);int e,h=0;
						for(e=1;e<strlen(cheq)-1;e++){
							pri[h]=cheq[e];h++;
						}
						//printf("%s\n",pri);
						if(strtok(cheq,".")!=NULL){
							printf("%s [incomplete]\n",pri);
						}
						
					}
				}
				
			}
			
		}
		else{
			printf("Invalid credentials!\n");
		}
	}
	else{
		printf("No Active Servers!\n");
	}
	

}
int main(int argc, char *argv[]){
	int i;
	char* command=(char*)malloc(100);
	char* filename=(char*)malloc(100);
	char* sub=(char*)malloc(100);
	//printf("%s",argv[1]);
	readConf(argv[1]);
	//for (i=0;i<4;i++)
	//	printf("%s %d %d\n",SERVERS[i],PORTS[i],serverCount);
	int bh=createSockets(-1);
	
	//send(sockets[3],"Hello",strlen("Hello"),0);*/
	//printf("%s %s\n",username,password);
	//flag=formConnection(username,password);
	
	while(1){
		bzero(command,sizeof(command));
		bzero(filename,sizeof(filename));
		command=operations();
		command=strtok(command," ");
		filename=strtok(NULL, " ");
		sub=strtok(NULL, " ");
			//printf("%s\n",command);
			//printf("%s\n",filename);
			//printf("%s\n",sub);
		if(strcmp(command,"list")==0){
			bzero(command,sizeof(command));
			strcpy(command,"lis");
		}
		for(i=0;i<serverCount;i++){
			int bytes = send(sockets[i],command,strlen(command),MSG_NOSIGNAL);
			if(bytes<=0){
				//printf("HI!! %d\n",i);
				int check=createSockets(i);
				if(check!=-1){
					int bytes = send(sockets[check],command,strlen(command),MSG_NOSIGNAL);
				}
			}
		}
		char *PATH = (char*)malloc(500);
		PATH=getenv("PWD");
		if(strcmp(command,"get")==0||strcmp(command,"GET")==0){
			//printf("I'm gonna GET!\n");
			getFileFromServer(PATH,filename,sub);
		}
		else if(strcmp(command,"put")==0||strcmp(command,"PUT")==0){
			//printf("Sending the file to server...!\n");
			putFileToServer(PATH,filename,sub);
			
		}
		else if(strcmp(command,"lis")==0||strcmp(command,"LIST")==0){
			//printf("I'm gonna LIST!\n");
			getListOfFiles(sub);
			
		}
		else{
			printf("Wrong Command, please enter again!\n");
			continue;
		}
	
	}
	
	
	
	return 0;
	
}
