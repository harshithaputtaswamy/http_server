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
	struct linger s1;
	s1.l_onoff = 1;
	s1.l_linger = 0;
	struct kevent event[5],change_event_tcp[5],change_event_udp[5];
	if(argc < 2){
		perror("Port number was not entered\n");
		exit(1);
	}
	
	sockfd_tcp = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd_tcp < 0){
		perror("ERROR creating socket");
		exit(1);
	}
	setsockopt(sockfd_tcp,SOL_SOCKET,SO_LINGER,&s1,sizeof(s1));
	
	sockfd_udp = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd_udp < 0){
		perror("ERROR creating socket");
		exit(1);
	}
	setsockopt(sockfd_udp,SOL_SOCKET,SO_LINGER,&s1,sizeof(s1));
	
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
	
	if(bind(sockfd_udp,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding udp\n");
		exit(1);
	}	
	
	printf("Socked binded\n");

	listen(sockfd_tcp,5);	

	clilen = sizeof(cli_addr);

	kq = kqueue();
	EV_SET(change_event_tcp, sockfd_tcp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);

	EV_SET(change_event_udp, sockfd_udp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);
	
	if(kevent(kq,change_event_tcp,1,NULL,0,NULL)== -1){
		perror("kevent");
		exit(1);
	}
	if(kevent(kq,change_event_udp,1,NULL,0,NULL)== -1){
		perror("kevent");
		exit(1);
	}
	printf("kqueue instialised\n");
	
	for(;;){
		new_ev = kevent(kq,NULL,0,event,1,NULL);
		if(new_ev == -1){
			perror("kevent");
			exit(1);
		}
		//printf("%d new_ev\n", new_ev);
		for(int i = 0; new_ev > i; i++){
			int eventfd = event[i].ident;
			
			if(event[i].flags & EV_EOF){
				printf("Client has disconnectesd\n");
				close(eventfd);
			}
			

			else if(eventfd == sockfd_udp){
				EV_SET(change_event_udp, eventfd, EVFILT_READ, EV_ADD, 0,0,NULL);
				//printf("udp event\n");
				memset(buffer,0,255);
				if(recvfrom(eventfd,(char *)buffer,255,0,(struct sockaddr *)&cli_addr,&clilen)<0){
					perror("ERROR reading\n");
					exit(1);
				}
				printf("udp Message from client: %s\n",buffer);
				if(sendto(eventfd,"Recieved your message-UDP\n",28,0,(const struct sockaddr *)&cli_addr,clilen)<0){
					perror("ERROR sending messaage udp\n");
					exit(1);
				}
				printf("Message sent udp\n\n\n");
			}
			else if(eventfd == sockfd_tcp){
				//printf("tcp ev\n");
				newsocket = accept(eventfd,(struct sockaddr *) &cli_addr,&clilen);
				if(newsocket < 0){
					perror("ERROR accepting connection\n");
					exit(1);
				}
				EV_SET(change_event_tcp, newsocket, EVFILT_READ, EV_ADD,0,0,NULL);
				if(kevent(kq,change_event_tcp,1,NULL,0,NULL)<0){
					perror("kevent error");
					exit(1);
				}
			//	continue;
			
			//printf("tcp event\n");
			
			}	
			else if(event[i].filter & EVFILT_READ){
				//printf("tcp recv\n");
				bzero(buffer,255);
				n = recv(eventfd,buffer,sizeof(buffer),0);
				if(n <0){
					perror("ERROR in reading message\n");
					exit(1);
				}
							
				printf("TCP The message from client is :%s\n",buffer);
					
				n = write(newsocket,"Recieved your message TCP\n",22);
				if(n<0){
					perror("ERROR writing to client\n");
					exit(1);
				}
				printf("TCP - Message sent\n");
			}
		}
	}
	return 0;
}
