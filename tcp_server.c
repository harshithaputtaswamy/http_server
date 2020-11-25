#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>

int main(int argc, char *argv[])
{
	int sockfd,portno,newsocket,clilen;
	char buffer[255];
	struct sockaddr_in server_addr, cli_addr;
	int n;
	if(argc < 2){
		perror("Port number was not entered\n");
		exit(1);
}
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("ERROR creating socket");
		exit(1);
	}
	bzero((char *) &server_addr, sizeof(server_addr));
	portno =atoi( argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	
	if(bind(sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding");
		exit(1);
	}
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsocket = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
	if(newsocket < 0){
		perror("ERROR accepting connection\n");
		exit(1);
	}

	bzero(buffer,255);
	n = read(newsocket,buffer,254);
	if(n <0){
		perror("ERROR in reading message\n");
		exit(1);
	}

	printf("The message from client is :%s\n",buffer);

	n = write(newsocket,"Recieved your message\n",22);
	if(n<0){
		perror("ERROR writing to client\n");
		exit(1);
	}

	return 0;


}
