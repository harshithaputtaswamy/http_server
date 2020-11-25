#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>

int main(int argc, char *argv[])
{
	int enable = 1;
	int tcp_sockfd,udp_sockfd,newsocket,clilen,portno,len,tcp_sock,udp_sock;
	char buffer[255];
	struct sockaddr_in server_addr, cli_addr;
	int n;
	if(argc < 2){
		perror("Port number was not entered\n");
		exit(1);
	}
	tcp_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(tcp_sockfd < 0){
		perror("ERROR creating socket");
		exit(1);
	}

	udp_sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(setsockopt(udp_sockfd,SOL_SOCKET,SO_REUSEPORT_LB, &enable, sizeof(int))<0)
		perror("setsockopt error:udp\n");
	if(udp_sockfd < 0){
		perror("ERROR creating socket");
		exit(1);
	}
	else{
	printf("Socket created");
	}
	bzero((char *) &server_addr, sizeof(server_addr));
	portno =atoi( argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	
/*	if(bind(tcp_sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding");
		exit(1);
*///	}
	if(bind(udp_sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding");
		exit(1);
	}
/*	listen(tcp_sockfd,5);
	clilen = sizeof(cli_addr);
	newsocket = accept(tcp_sockfd,(struct sockaddr *) &cli_addr,&clilen);
	if(newsocket < 0){
		perror("ERROR accepting connection\n");
		exit(1);
*///	}

	bzero(buffer,255);
/*	n = recv(newsocket,(char *)buffer,254,0);
	if(n <0){
		perror("ERROR in reading message-tcp\n");
		exit(1);
	}
	else
		printf("Message recieved\n");
	printf("The message from client is :%s\n",buffer);
*/	
	bzero(buffer,255);
	len = sizeof(cli_addr);
	//n = recvfrom(udp_sockfd,(char *)buffer,254,0,(struct sockaddr *)&cli_addr,&len);
	//buffer[n]='\0';
	if((recvfrom(udp_sockfd,(char *)buffer,254,0,(struct sockaddr *)&cli_addr,&len)) <0){
		perror("ERROR in reading message-udp\n");
		exit(1);
	}
	else
		printf("Message recieved\n");
	printf("The message from client is :%s\n",buffer);

/*	n = send(newsocket,"Recieved your message\n",22,MSG_DONTWAIT);
	if(n<0){
		perror("ERROR writing to client\n");
		exit(1);
*///	}
	n = sendto(udp_sockfd,"Recieved your message\n",22,MSG_DONTWAIT,(const struct sockaddr *)&cli_addr,sizeof(cli_addr));
	if(n<0){
		perror("ERROR writing to client\n");
		exit(1);
	}
	printf("Message sent\n");
	return 0;


}
