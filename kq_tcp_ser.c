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
	sockfd_tcp = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd_tcp < 0){
		perror("ERROR creating socket");
		exit(1);
	}
/*	sockfd_udp = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd_udp < 0){
		perror("ERROR creating socket");
		exit(1);
	}*/
	printf("Socket created\n");
	bzero((char *) &server_addr, sizeof(server_addr));
	portno = atoi( argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	
	if(bind(sockfd_tcp,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding");
		exit(1);
	}
	printf("Socked binded\n");
	listen(sockfd_tcp,5);	
	clilen = sizeof(cli_addr);

	kq = kqueue();
	EV_SET(change_event, sockfd_tcp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);

	//EV_SET(change_event, sockfd_udp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);
	
	if(kevent(kq,change_event,1,NULL,0,NULL)== -1){
		perror("kevent");
		exit(1);
	}
	printf("kqueue instialised\n\n");
	for(;;){
		new_ev = kevent(kq,NULL,0,event,1,NULL);
		if(new_ev == -1){
			perror("kevent");
			exit(1);
		}
		//printf("%d new_ev\n", new_ev);
		for(int i = 0; new_ev > i; i++){
			int eventfd = event[i].ident;
			//printf("Entered into for\n");
			if(event[i].flags & EV_EOF){
				printf("Client has disconnectesd\n");
				close(eventfd);
			}
			
			else if(eventfd == sockfd_tcp){
				newsocket = accept(eventfd,(struct sockaddr *) &cli_addr,&clilen);
				if(newsocket < 0){
					perror("ERROR accepting connection\n");
					exit(1);
				}
				EV_SET(change_event, newsocket, EVFILT_READ, EV_ADD,0,0,NULL);
				if(kevent(kq,change_event,1,NULL,0,NULL)<0){
					perror("kevent error");
					exit(1);
				}
				continue;
			}
		
			else if(event[i].filter & EVFILT_READ){
				bzero(buffer,255);
				n = recv(eventfd,buffer,sizeof(buffer),0);
				if(n <0){
					perror("ERROR in reading message\n");
					exit(1);
				}
					
				printf("The message from client is :%s\n",buffer);
					
				n = write(newsocket,"Recieved your message\n",40);
				if(n<0){
					perror("ERROR writing to client\n");
					exit(1);
				}
				printf("Message sent\n");
			}
		}
	}
	return 0;
}
