#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/event.h>
int main(int argc, char *argv[])
{
	int sockfd_tcp,sockfd_udp,portno,newsocket,clilen,kq,new_ev;
	char buffer[255];
	struct sockaddr_in server_addr, cli_addr;
	int n;
	struct kevent event[5],change_event[5];
	if(argc < 2){
		perror("Port number was not entered\n");
		exit(1);
	}

	sockfd_udp = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd_udp < 0){
		perror("ERROR creating socket");
		exit(1);
	}
	printf("Socket created\n");
	bzero((char *) &server_addr, sizeof(server_addr));
	portno = atoi( argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	
	if(bind(sockfd_udp,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding");
		exit(1);
	}
	printf("Socked binded\n");
	
	clilen = sizeof(cli_addr);

	kq = kqueue();

	EV_SET(change_event, sockfd_udp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);
	
	if(kevent(kq,change_event,1,NULL,0,NULL)== -1){
		perror("kevent");
		exit(1);
	}
	for(;;){
		new_ev = kevent(kq,NULL,0,event,1,NULL);
		if(new_ev == -1){
			perror("kevent");
			exit(1);
		}
		//printf("%d events\n",new_ev);
		for(int i = 0; new_ev > i; i++){
			int eventfd = event[i].ident;
			//printf("for loop\n");
			if(event[i].flags & EV_EOF){
				printf("Client has disconnectesd\n");
				close(eventfd);
			}
			else if(eventfd == sockfd_udp){
				EV_SET(change_event, eventfd, EVFILT_READ, EV_ADD,0,0,NULL);
			//	if(kevent(kq,change_event,1,NULL,0,NULL)<0)
			//		perror("kevent error");
			//	printf("sock\n");
			//	printf("%s filter",event[i].filter);
			//}
	
			//else if(event[i].filter & EVFILT_READ){
			//	printf("here here\n");
				memset(buffer,0,255);
		
			//	printf("here\n");
				n = recvfrom(eventfd,(char *)buffer,255,0,(struct sockaddr *)&cli_addr,&clilen);
				if(n<0){
					perror("ERROR in reading - udp\n");
					exit(1);
				}
				printf("The message from client is :%s\n",buffer);
					
				if(sendto(eventfd,"Recieved your message - UDP\n",28,0,(const struct sockaddr *)&cli_addr,clilen)<0){
					perror("Message not sent - UDP");
					exit(1);
				}
				printf("Message sent\n");
			}
		}
	}
	return 0;
}
