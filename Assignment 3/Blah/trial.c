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

int main(){
	FILE *fd;
	fd=fopen("/home/rajath/Desktop/NetCys/server/DFS2","r");
	if(fd==NULL){
		perror("Lol");
	}
	fclose(fd);

}
