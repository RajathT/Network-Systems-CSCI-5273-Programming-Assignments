#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<poll.h>


#define BUFFSIZE 4096
#define PACKETSIZE 2048
//***********************CLIENT PROGRAM*************************

//**********Recieves the retrasmitted packets*******************
//If packets donot some in sequence they are re transmitted

int checkforACK(int sock, struct sockaddr_in *myServer,char *seqNumchar,int start,int end,char *curr_char);

//*****************Checks for Sequence Numbers of the packets*****
//Returns 1 is in sequence else calls recievepackets function
int resend(int seqNum,char*seqNumchar,int start,int end,int sock,struct sockaddr_in *myServer,char *buffer){
	long a,k=0,j,flag;char *curr_char = (char *)malloc(PACKETSIZE+1);
	struct sockaddr_in myServer_addr=*myServer;int myServerlen = sizeof(myServer_addr);
	for(a=0;a<6;a++){
		curr_char[k]=seqNumchar[a];k++;
	}
	for(j=start; j<end; j++){
    		curr_char[k] = buffer[j];
		printf("%ld %ld %c\n",k,j,curr_char[k]);k++;
  	}

	if(sendto(sock, curr_char, PACKETSIZE, 0, (struct sockaddr *)&myServer_addr, myServerlen)<0){
		perror("Couldn't send the message");
		exit(1);
	}
	
	flag=checkforACK(sock,&myServer_addr,seqNumchar,start,end,buffer);
	return 1;	
}
//********************Recieves the file and stores it***********
//Packets are checked with their ACK's = SeqNumbers
//Seqnumbers are passed as a string in front of every packets
int checkforACK(int sock, struct sockaddr_in *myServer,char *seqNumchar,int start,int end,char *curr_char){
	
	struct sockaddr_in myServer_addr=*myServer;char *ACK=(char*)malloc(100);
	int myServerlen = sizeof(myServer_addr);int a;
    	if(recvfrom(sock, ACK, 100, 0, (struct sockaddr *)&myServer_addr, &myServerlen)>0){			
		printf("%s",ACK);
	}
	printf("checkforack %d %d\n",atoi(seqNumchar),atoi(ACK));
	if(atoi(seqNumchar)==atoi(ACK)){
		char *status=(char *)malloc(3);status="yes";
		if(sendto(sock, status, 3, 0, (struct sockaddr *)&myServer_addr, myServerlen)<0){
				perror("Couldn't send the message");
				exit(1);
		}
		return 1;
	}
	
	else{	
		int seqNum=atoi(seqNumchar);
		a=resend(seqNum,seqNumchar,start,end,sock,&myServer_addr,curr_char);
		return a;
	}		
}
//****************Sends file to the Server******************
//Packets are sent with the sequence sumbers with Packet size of 2048
char* convertToString(int number){
	char *seqchar,seqNum[6]; unsigned int a[6],count=1,seqNumber;
	seqchar = (char*)malloc(6);

	sprintf(seqNum,"%d",number);
	seqNumber=number;
	while(seqNumber/10){
		a[count]=seqNumber%10;
		seqNumber=seqNumber/10;count=count+1;
	}
	count=6-count;
	memset(seqchar,'0',count);
	strcpy(seqchar+count,seqNum);
	return seqchar;
}
//***************Sends the file to the server
void sendServerFile(char* command,char* filename,int sock,struct sockaddr_in *myServer){
	printf("In put server Client command");
	int myServerlen, packets, lpackets,bytes,llpackets, start,startre, end,endre,k=0,q=0,a,flag=1;
	long i,j,filesize,count=0,size;
	struct sockaddr_in myClient_addr;
	struct sockaddr_in myServer_addr=*myServer;
	myServerlen = sizeof(myServer_addr);
		
	FILE * file;char *buffer, *curr_char,*seqNumchar,*result;
	file = fopen(filename, "rb");
	if(NULL==file){
		printf("File Not Found\n");exit(1);
	}
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, SEEK_SET);
	rewind(file);
	buffer = (char *)malloc(filesize+1);
	bytes=fread(buffer, filesize, 1, file);
	curr_char = (char *)malloc(PACKETSIZE+1);
	seqNumchar = (char *)malloc(100);
	
	packets = filesize/(PACKETSIZE-6);
		
	seqNumchar=convertToString(packets+1);
	
	result=seqNumchar;
	start=0;end=PACKETSIZE-7;
	
	
	for(i=1;i<=packets+1;i++){
		if(i<=packets){
			seqNumchar=convertToString(i);
			for(a=0;a<6;a++){
				curr_char[k]=seqNumchar[a];k++;
			}
			startre=start;endre=end+1;
			for(j=start; j<=end; j++){
    				curr_char[k] = buffer[j];
				//printf("%d %ld %c\n",k,j,curr_char[k]);k++;
  			}
			start=end+1;
			end=end+PACKETSIZE-6;
			k=0;
			if(sendto(sock, curr_char, PACKETSIZE, 0, (struct sockaddr *)&myServer_addr, myServerlen)<0){
				perror("Couldn't send the message");
				exit(1);
			}
			
			flag=checkforACK(sock,&myServer_addr,seqNumchar,startre,endre,buffer);
			if(flag==1){
				flag=0;continue;
			}
			
		}
		if(i>packets){
			lpackets=filesize-packets*(PACKETSIZE-6);k=0;
			if(lpackets){
				end=end-(PACKETSIZE-7);
				llpackets = lpackets+end;lpackets=lpackets+6;
				seqNumchar=convertToString(i);
				startre=end;endre=llpackets;
				for(a=0;a<6;a++){
					curr_char[k]=seqNumchar[a];k++;
				}
				for(j=end; j<llpackets; j++){
    					curr_char[k] = buffer[j];
					printf("%ld %c\n",j,curr_char[k]);k++;
  				}
				
				if(sendto(sock, curr_char, lpackets, 0, (struct sockaddr *)&myServer_addr, myServerlen)<0){
					perror("Couldn't send the message");
					exit(1);
				}
				flag=checkforACK(sock,&myServer_addr,seqNumchar,startre,endre,buffer);
				if(flag==1){
					flag=0;continue;
				}
				
			}
		}
		
	}

		fclose(file);		
}
//**************Recieving the file from the server
void recvFileFromServer(char* filename,int sock,struct sockaddr_in *myClient){	
	
	int i=0,j=0,count=1,myClient_len,flag=0;
	long len=0,nbytes,startre,endre;		
	char *buffer,*bufferdup,*seqNumchar;
	FILE *file;	
	buffer = (char *)malloc(BUFFSIZE);
	bufferdup = (char *)malloc(BUFFSIZE);
	seqNumchar = (char *)malloc(6);		
	bzero(bufferdup, BUFFSIZE);
	struct sockaddr_in myServer_addr=*myClient;
	struct sockaddr_in myClient_addr=*myClient;
	myClient_len = sizeof(myClient_addr);
	while(1){
		if((nbytes=recvfrom(sock, buffer, PACKETSIZE, 0, (struct sockaddr *)&myClient_addr, &myClient_len))<0){
			perror("recvfrom!");			
			exit(1);
		}
		startre=len+j;endre=nbytes-6;
		while(i<6){
			seqNumchar[i]=buffer[i];i++;
		}		
		while(j<nbytes-6){
			bufferdup[len+j]=buffer[i];
			printf("%ld %d %d %ld %c %c\n",nbytes,j,i,len+j,bufferdup[len+j],buffer[i]);
			i++;j++;
		}
		len=len+j;j=0;i=0;count=count+1;
		
		if(nbytes<PACKETSIZE){
			printf("Im here %ld",nbytes);
			break;
		}
	}
		
	file = fopen(filename, "wb");
	if(NULL==file)
		printf("File problem");
	fwrite(bufferdup, len,1, file);
	fseek(file,0,SEEK_END);
	printf("Size of file After: %zu\n",ftell(file));
}
//Function that helps take the client commands
void getClientcommands(char*command,char*filename,int sock,struct sockaddr_in *myServer){
	char *choice; choice=(char*)malloc(6);int i=0,j=0;
	struct sockaddr_in myServer_addr=*myServer;
	int myServer_len = sizeof(myServer_addr);FILE *flist;
	char fileChar;
	printf("%s|%s\n",command,filename);
	if(sendto(sock, command, 6, 0, (struct sockaddr *)&myServer_addr, myServer_len)<0){
			perror("Couldn't send the message");
			exit(1);
	}
	if(sendto(sock, filename, 30, 0, (struct sockaddr *)&myServer_addr, myServer_len)<0){
			perror("Couldn't send the message");
			exit(1);
	}
	if(strcmp(command,"get")==0){
		recvFileFromServer(filename,sock,&myServer_addr);
	}
	if(strcmp(command,"put")==0){
		sendServerFile(command,filename,sock,&myServer_addr);
	}
	if(strcmp(command,"delete")==0){
	}
	if(strcmp(command,"ls")==0){
		filename="store.txt";
		recvFileFromServer(filename,sock,&myServer_addr);
		flist=fopen("store.txt","r");
		if(flist==NULL){
			printf("No Data");
		}
		fileChar=fgetc(flist);
		while(fileChar != EOF){
			printf("%c",fileChar);
			fileChar=fgetc(flist);
		}
		printf("\n");
		fclose(flist);
	}
	if(strcmp(command,"exit")==0){
	}
}
	
int main(int argc, char *argv[]){
	
	if(argc < 3){
		printf("Please enter correct arguments");
		exit(1);
	}
	
	struct sockaddr_in myServer_addr;
	struct hostent *myServer;
	int sock, port_no, choice, myServerlen,i=0;char *filename, *command; 
	
	
	if((sock = socket(AF_INET, SOCK_DGRAM, 0))<0){
		perror("Socket creation failed");
		exit(1);
	}

	port_no = atoi(argv[2]);
	if(port_no>65535){
		printf("Wrong Port Number,Enter less than 65535");
		exit(1);
	}
	myServer = gethostbyname(argv[1]);

	bzero((char *) &myServer_addr, sizeof(myServer_addr));

	memset((char *)&myServer_addr,0,sizeof(myServer_addr));
	myServer_addr.sin_family = AF_INET;
	bcopy((char *)myServer->h_addr, (char *)&myServer_addr.sin_addr.s_addr, myServer->h_length);
	myServer_addr.sin_port = htons(port_no);
	myServerlen = sizeof(myServer_addr);
	
	filename = (char *)malloc(30);
	command = (char *)malloc(30);
	
	while(1){
		printf("Enter your command:\n");
		scanf("%s",command);
		if(command[0]=='g'||command[0]=='p'||command[0]=='d')
			scanf("%s",filename);
	
		getClientcommands(command,filename,sock,&myServer_addr);
	}

	return 0;
}
