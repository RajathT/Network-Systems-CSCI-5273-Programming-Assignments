#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<poll.h>
#include <arpa/inet.h>

#define BUFFSIZE 104857600
#define PACKETSIZE 2048
//***********************SERVER PROGRAM*************************

//**********Recieves the retrasmitted packets*******************
//If packets donot some in sequence they are re transmitted
void recievepackets(int sock,char* seqNumchar,struct sockaddr_in *myClient,int start,int end,char* bufferdup){
	int j=0,nbytes,i=0;
	struct sockaddr_in myClient_addr=*myClient;
	int myClient_len = sizeof(myClient_addr);
	char*buffer=(char*)malloc(BUFFSIZE);
	if((nbytes=recvfrom(sock, buffer, PACKETSIZE, 0, (struct sockaddr *)&myClient_addr, &myClient_len))<0){
		perror("recvfrom!");			
		exit(1);
	}
	while(i<6){
		seqNumchar[i]=buffer[i];i++;
	}		
	while(i<end){
		bufferdup[start+j]=buffer[i];
		i++;j++;
	}
}
//*****************Checks for Sequence Numbers of the packets*****
//Returns 1 is in sequence else calls recievepackets function
int checkpackets(int sock,char* seqNumchar,struct sockaddr_in *myClient,int start,int end,char* buffer){
	printf("in check\n");
	int res;char *status = (char *)malloc(3);
	struct sockaddr_in myClient_addr=*myClient;
	int myClient_len = sizeof(myClient_addr);
	if(recvfrom(sock, status, 3, 0, (struct sockaddr *)&myClient_addr, &myClient_len)<0){
		perror("recvfrom!");			
		exit(1);
	}
	printf("%s\n",status);
	if(status[0]=='y'){
		printf("%s\n",status);
		return 1;
	}
	
	else{
		recievepackets(sock,seqNumchar,&myClient_addr,start,end,buffer);
	}
}
//********************Recieves the file and stores it***********
//Packets are checked with their ACK's = SeqNumbers
//Seqnumbers are passed as a string in front of every packets
void putClientFile(int packets,char* filename,int sock,struct sockaddr_in *myClient){	
	printf("In put Client command\n");
	int i=0,j=0,flag,count=1,myClient_len,recvpackets=0,recvpack[packets];
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
			//printf("%ld %d %d %ld %c %c\n",nbytes,j,i,len+j,bufferdup[len+j],buffer[i]);
			i++;j++;
		}
		len=len+j;j=0;i=0;count=count+1;
		printf("nbytes %ld ",nbytes);	
		if(sendto(sock, seqNumchar, 100, 0, (struct sockaddr *)&myClient_addr, myClient_len)<0){
			perror("Couldnt send ACK");			
			exit(1);
		}
		//printf("Recieved %s \n",seqNumchar);
		flag=checkpackets(sock,seqNumchar,&myClient_addr,startre,endre,bufferdup);
		
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
//Converts the packet numbers to string
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
//****************Sends file to the Client******************
//Packets are sent with the sequence sumbers with Packet size of 2048
void sendFileToClient(char* command,char* filename,int sock,struct sockaddr_in *myServer){
	
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
	strcat(seqNumchar,command);
	result=(char*)malloc(strlen(command)+6);
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
				printf("%d %ld %c\n",k,j,curr_char[k]);k++;
  			}
			start=end+1;
			end=end+PACKETSIZE-6;
			k=0;
			if(sendto(sock, curr_char, PACKETSIZE, 0, (struct sockaddr *)&myServer_addr, myServerlen)<0){
				perror("Couldn't send the message");
				exit(1);
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
			}
		}
		
	}
	
}
//Function that helps take the client commands
void getClientcommands(int sock,struct sockaddr_in *myClient){
	
	struct sockaddr_in myClient_addr=*myClient;
	int myClient_len = sizeof(myClient_addr),i=0,packets;
	char* command=(char *)malloc(6);
	char* filename=(char *)malloc(30);
	char * seqNumchar=(char *)malloc(6);
	struct timeval timeout={0,0};
	while(1){
		int timeset = setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
	
		if(recvfrom(sock, command, 6, 0, (struct sockaddr *)&myClient_addr, &myClient_len)<0){			
			perror("recvfrommmm!");			
			exit(1);
		}
		if(recvfrom(sock, filename, 30, 0, (struct sockaddr *)&myClient_addr, &myClient_len)<0){			
			perror("recvfrom!");			
			exit(1);
		}
		printf("%s|%s\n",command,filename);
		if(strcmp(command,"get")==0){
			sendFileToClient(command,filename,sock,&myClient_addr);
			
		}
		if(strcmp(command,"put")==0){
			putClientFile(packets,filename,sock,&myClient_addr);
		
		}
		if(strcmp(command,"delete")==0){
			remove(filename);
			
		}
		if(strcmp(command,"ls")==0){
			printf("Yolo ls");
			system("ls > files.txt");
			filename="files.txt";
			sendFileToClient(command,filename,sock,&myClient_addr);
		}
		if(strcmp(command,"exit")==0){
			exit(0);
		}
		
	}
	
}	
int main(int argc, char *argv[]){
	
	struct sockaddr_in myClient_addr;
	int sock, port_no;
	if(argc<2){
		printf("Invalid arguments");
		printf("Format: ./server [port]");
	}
	if((sock = socket(AF_INET, SOCK_DGRAM, 0))<0){
		perror("Socket creation failed");
		exit(1);
	}
	port_no=atoi(argv[1]);
	if(port_no>65535){
		printf("Invalid port number");
		exit(1);
	}
	bzero((char *) &myClient_addr, sizeof(myClient_addr));
	memset((char *)&myClient_addr,0,sizeof(myClient_addr));
	myClient_addr.sin_family = AF_INET;
	myClient_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	myClient_addr.sin_port = htons(port_no);
	
	if(bind(sock, (struct sockaddr *) &myClient_addr, sizeof(myClient_addr))<0){
		perror("Socket binding failed");
		exit(1);
	}
	
	getClientcommands(sock,&myClient_addr);
	return 0;
}


