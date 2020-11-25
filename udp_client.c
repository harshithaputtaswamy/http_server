#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	int len,n,sockfd, portno;
	char *msg = "Message to server from client\n";
	char buffer[256];
	struct sockaddr_in server_addr;
	if(argc<2){
		perror("Port not entered\n");
		exit(1);
	}
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0){
		perror("Error creating socket\n");
		exit(1);
	}
	else
		printf("socket created\n");
	portno = atoi(argv[1]);
	memset(&server_addr,0,sizeof(server_addr));
	
	
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	server_addr.sin_family = AF_INET;
	
	len = sizeof(server_addr);
	sendto(sockfd,(const char *)msg,strlen(msg),0,(const struct sockaddr *)&server_addr,len);
	if(sendto < 0){
		perror("Message not sent\n");
		exit(1);
	}
	printf("Message sent\n"); 
	n = recvfrom(sockfd,(char *)buffer,255,0,(struct sockaddr *)&server_addr,&len);
	buffer[n]='\0';
	if(n<0){
		perror("Message not recieved\n");
		exit(1);
	}
	printf("Server messaage : %s\n",buffer);
	
	return 0;

}

