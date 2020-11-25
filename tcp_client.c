#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>


int main(int argc, char *argv[])
{
	int tcp_sock,udp_sock,tcp_sockfd,udp_sockfd,portno;
	char *msg="hi from client";
	char buffer[256];
	struct sockaddr_in server_addr;
	int enable = 1;
	int n;
	if(argc < 2){
		perror("Port number was not entered\n");
		exit(1);
	}
	tcp_sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(tcp_sockfd < 0){
		perror("ERROR creating socket-tcp");
		exit(1);
	}
	/*udp_sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(setsockopt(udp_sockfd,SOL_SOCKET,SO_REUSEPORT_LB, &enable, sizeof(int))<0)
		perror("setsockopt error udp\n");
	if(udp_sockfd < 0){
		perror("ERROR creating socket-udp");
		exit(1);
	}*/

	portno = atoi(argv[1]);
	
	
	bzero((char *)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(portno);
	

	if(connect(tcp_sockfd,(const struct sockaddr *)&server_addr,sizeof(server_addr))<0){
		perror("ERROR connecting-tcp");
		exit(1);
	}
	/*if(connect(udp_sockfd,&server_addr,sizeof(server_addr))<0){
		perror("ERROR connecting-udp");
		exit(1);
	}*/
	n = send(tcp_sockfd,(const char *)msg,strlen(msg),0);
	if(n <0){
		perror("ERROR in writing to server-tcp\n");
		exit(1);
	}
/*	n = sendto(udp_sockfd,(const char *)msg,strlen(msg),0,(const struct sockaddr *)&server_addr,sizeof(server_addr));
	if(n <0){
		perror("ERROR in writing to server-udp\n");
		exit(1);
	}
*/
	printf("The message sent is: %s\n",msg);

	bzero(buffer,255);
	if(recv(tcp_sockfd,(char *)buffer,254,0)<0){
		perror("message not recieved\n");
		exit(1);
	}
	printf("Server: %s\n",buffer);
	return 0;


}
