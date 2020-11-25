#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>

int main(int argc, char *argv[]){
	int len,n,sockfd, portno;
	char *msg = "Message from server\n";
	char buffer[256];
	int enable = 1;
	struct sockaddr_in server_addr, cli_addr;
	if(argc<2){
		perror("Port not defined\n");
		exit(1);
	}
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0){
		perror("Error creating socket\n");
		exit(1);
	}
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT_LB,&enable,sizeof(int))<0)
		perror("setsockopt failed\n");
	portno = atoi(argv[1]);
	memset(&server_addr,0,sizeof(server_addr));
	memset(&cli_addr,0,sizeof(cli_addr));
	
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	server_addr.sin_family = AF_INET;
	if(bind(sockfd,(const struct sockaddr *)&server_addr, sizeof(server_addr))<0){
		perror("Bind failed\n");
		exit(1);
	}
	len = sizeof(cli_addr);
	n = recvfrom(sockfd,(char *)buffer,255,0,(struct sockaddr *)&cli_addr,&len);
	buffer[n]='\0';
	if(recvfrom <0){
		perror("Message not recieved\n");
		exit(1);
	}
	printf("Client messaage : %s",buffer);
	sendto(sockfd,(const char *)msg,strlen(msg),0,(const struct sockaddr *)&cli_addr,len);
	if(sendto < 0){
		perror("Message not sent\n");
		exit(1);
	}
	printf("Message sent\n");
	return 0;

}

