#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/event.h>
#include<sys/stat.h>
#include<time.h>
#define EOL "\r\n"
#define eol_size 2


const char* parse_request(const char *raw);
void stringcpy(char*, const char*, char);
void stringcat(char*, char*);

void hexDump(const char *buf, const void *addr, const int len){
	int i;
	unsigned char buff[100];
	char *tok = NULL;
	int buf_len=16;
	const unsigned char *pc = (const unsigned char *)addr;
	if(buf != NULL)
		printf("%s:\n",buf);
	if(len <= 0){
		printf("Length zero\n");
		return;
	}
	for(i=0; i<len;i++){
		//tok = strtok(buff,"\n");
		//while(tok != NULL)
		if((i%16)==0)
	//	if(buff=='\n')
		{
			if(i!=0)
				printf("  %s\n",buff);
			printf("  %04x",i);
			//tok = strtok(NULL, "\n");
		}
		printf("  %02x",pc[i]);
		//buf_len = sizeof(buff);
		if((pc[i]>0x20)||(pc[i]<0x7e))
		//	buff[i%buf_len]='.';
		//else
			buff[i%buf_len]=pc[i];
		buff[(i%buf_len)+1]='\0';
	}
	while((i%buf_len)!=0){
		printf("	");
		i++;
	}
	printf("  %s\n",buff);
}


void stringcpy(char *dest, const char *src, char c){
	while(*src != c)
		*dest++ = *src++;
	*dest = '\0';
	return;
}

void stringcat(char *dest, char *src){
	while(*src){
		*dest++ = *src++;
	}
	return;
}

const char* parse_request(const char *raw){
	FILE *fptr;
	char *res;
	char *file;
	char *http_v;
	int content_len;
	char *status_code;
	char *status_msg;
	char *body;
	char *content_type;
	char *connection;
//	char *result;

	res = malloc(1024);

	//http method
	size_t method_len= strcspn(raw," ");

	char method[method_len];
	strncpy(method,raw,method_len);

	raw += method_len+2;
//	printf("%d raw",raw);
	
	//http request url
	size_t file_len = strcspn(raw, " ");

//	raw += 1;
	stringcpy(file,raw,' ');	
//	printf("%s\n",file);
	raw += file_len + 1;
	
	//http request version
	size_t ver_len = strcspn(raw,EOL);

	stringcpy(http_v, raw, '\r' );	
//	http_v = "HTTP/1.1";
//	printf("%s\n",http_v);	
//	stringcat(result, http_v);
	raw += ver_len +2;

	//file content	

	fptr = fopen(file,"r+");

	if(fptr == NULL){
		body = "Cannot open file\r\n";
		status_code = " 404";
		status_msg = " Not Found\r\n";		
		content_len = 16;
	}
	

	const char *type = strrchr(file,'.');
//	printf("%s\n",type);
	if(type && ".html")
		content_type="text/html\r\n";
	else if(type && ".jpeg")
		content_type = "image/jpeg\r\n";
	else if(type && ".txt")
		content_type = "text/plain\r\n";
	else if(type && ".ico")
		content_type = "image/x-icon\r\n";

	
	int n = 0;
	if(fptr != NULL){
		body = malloc(1024);	
		do{
			body[n] = fgetc(fptr);
			if(feof(fptr))
				break;
			n+=1;		
		}while(1);
	
		body[n]='\0';
		status_code = " 200";
		status_msg = " OK\r\n";
		content_len = n;	
	}

//	stringcat(result,status_code);
//	stringcat(result, status_msg);

	connection = "Closed\r\n";
//	printf("%s",result);
	sprintf(res,"%s%s%sContent-Type: %sContent-Length: %d\r\nConnection: %s\r\n%s",http_v,status_code,status_msg,content_type,content_len,connection,body);
	return res;

}


int main(int argc, char *argv[])
{
	int E = 1;
	struct linger sl;
	sl.l_onoff = 1;
	sl.l_linger = 0;
	int sockfd_tcp,sockfd_udp,portno,newsocket,clilen,kq,new_ev;
	char buffer[1000];
	const char *httpHeader; 
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
	setsockopt(sockfd_tcp,SOL_SOCKET,SO_LINGER,&sl,sizeof(sl));
	setsockopt(sockfd_tcp,SOL_SOCKET,SO_REUSEADDR,&E,sizeof(int));
	setsockopt(sockfd_tcp,SOL_SOCKET,SO_REUSEPORT,&E,sizeof(int));
	bzero((char *) &server_addr, sizeof(server_addr));
	portno = atoi( argv[1]);
//	portno = 8200;	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	
	if(bind(sockfd_tcp,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
		perror("ERROR binding");
		exit(1);
	}
	//printf("Socked binded\n");
	listen(sockfd_tcp,5);	
	clilen = sizeof(cli_addr);

	kq = kqueue();
	EV_SET(change_event, sockfd_tcp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);

		
	if(kevent(kq,change_event,5,NULL,0,NULL)== -1){
		perror("kevent");
		exit(1);
	}
	//printf("kqueue instialised\n\n");
	for(;;){
		new_ev = kevent(kq,NULL,0,event,1,NULL);
		if(new_ev == -1){
			perror("kevent");
			exit(1);
		}
		printf("%d new_ev\n", new_ev);
		for(int i = 0; i<new_ev; i++){

			//printf("%d",i);
			int eventfd = event[i].ident;
			
			if(event[i].flags & EV_EOF){
				printf("Client has disconnected\n\n");
				close(eventfd);
			}
			
		//	printf("eventfd : %d\n",eventfd);
			if(eventfd == sockfd_tcp){
				
			//	printf("eventfd : %d\n",eventfd);
				newsocket = accept(eventfd,(struct sockaddr *) &cli_addr,&clilen);
			//	printf("accepted\n");
				if(newsocket < 0){
					perror("ERROR accepting connection\n");
					exit(1);
				}
				EV_SET(change_event, newsocket, EVFILT_READ, EV_ADD,0,0,NULL);
				if(kevent(kq,change_event,1,NULL,0,NULL)<0){
					perror("kevent error");
					exit(1);
				}
			//	printf("eventfd : %d\n",eventfd);
				continue;
			}
		
			else if(event[i].filter == EVFILT_READ){
				bzero(buffer,500);
				printf("HERE&&&&&&&&&&&%d\n", event[i].ident);
				n = read(eventfd,buffer,sizeof(buffer));
				if(n <0){
					perror("ERROR in reading message\n");
					exit(1);
				}
				printf("%s",buffer);

				
				EV_SET(change_event, event[i].ident,EVFILT_READ, EV_DELETE,0,0,0);
				kevent(kq,change_event,1,NULL,0,NULL);

				EV_SET(change_event,eventfd, EVFILT_WRITE, EV_ADD, 0, 0, 0);
				kevent(kq,change_event,1,NULL,0,NULL);

			/*	httpHeader = parse_request(buffer);

				n = write(eventfd,httpHeader,strlen(httpHeader));
				if(n<0){
					perror("ERROR writing to client\n");
					exit(1);
				}
				printf("Message sent is:\n%s\n",httpHeader);
			*/
			}
			else if(event[i].filter == EVFILT_WRITE){
				httpHeader = parse_request(buffer);

				n = write(eventfd,httpHeader,strlen(httpHeader));
				if(n<0){
					perror("ERROR writing to client\n");
					exit(1);
				}
				printf("Message sent is:\n%s\n",httpHeader);
				

				EV_SET(change_event, newsocket, EVFILT_WRITE,EV_DELETE,0,0,0);
				kevent(kq, change_event,1,NULL,0,NULL);
				close(eventfd);
				

			}		

		}

	}
	return 0;
}
