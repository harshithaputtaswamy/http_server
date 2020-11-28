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

const char* parse_request(const char *raw){
	FILE *fptr;
	char *f_char;
	char *res;
	char *method;
	char *file;
	char *file_path;
	char *http_v;
	int content_len;
	char *status_code;
	char *status_msg;
	char *body;
	char *content_type;
	char *connection;


	res = malloc(800);

	//http method
	size_t method_len= strcspn(raw," ");
	if(memcmp(raw, "GET", strlen("GET"))==0)
		method = "GET";
	else if(memcmp(raw, "HEAD", strlen("HEAD"))==0)
		method = "HEAD";
	else
		method = "UNSUPPORTED";
	raw += method_len+1;
	
//	printf("method : %s\n",method);
	
	//http request url
	size_t file_len = strcspn(raw, " ");
//	printf("%d: ",file_len);
	file = malloc(file_len-1);
	file_path = malloc(file_len);

	memcpy(file_path,raw,file_len);
	file_path[file_len] = '\0';
//	printf("%s",file_path);

	raw += 1;
	
	memcpy(file,raw,file_len-1);
	file[file_len-1]='\0';
//	printf("\n%s",file);
	raw += file_len ;
	
	//http request version
	size_t ver_len = strcspn(raw,EOL);
	http_v = malloc(ver_len);
	//printf("version mem\n");

	memcpy(http_v,raw,ver_len);
	http_v[ver_len]='\0';
	raw += ver_len +2;
	

	//file content
	
//	printf("file path:%s\n",file_path);
	fptr = fopen(file,"r+");
//	printf("file failed");
	if(fptr == NULL){
		//char body[1000];
		body = "Cannot open file\r\n";

		status_code = malloc(4);
		memcpy(status_code," 404",4);
		status_code[4]='\0';
		status_msg = malloc(13);
		memcpy(status_msg," Not Found\r\n",12);
		status_msg[12]='\0';		
		content_len = 16;
	
	}
	
	char *type;


	type = strchr(file_path,'.');
	//printf("209");	

//	char *body;

	if(strcmp(type,".html") == 0)
		content_type="text/html\r\n";
	else if(strcmp(type, ".jpeg") == 0)
		content_type = "image/jpeg\r\n";
	else if(strcmp(type, ".txt") == 0)
		content_type = "text/plain\r\n";
	else if(strcmp(type, ".ico") == 0)
		content_type = "image/x-icon\r\n";
	int type_len = strlen(content_type);
	//content_type[type_len]='\0';
//	printf("\ntype : %s",content_type);
	
	f_char = malloc(2);

	int n = 0;
	if(fptr != NULL){
//		char *body;
		n=2;
		body = malloc(n);	
		do{
			f_char[0]=fgetc(fptr);
			if(feof(fptr))
				break;
			n+=1;
			body = realloc(body,n);
			strcat(body,f_char);
	}
		while(1);
		int body_len = strlen(body);
		body[body_len]='\0';
	
		status_code = malloc(4);
		memcpy(status_code," 200",4);
		status_code[4]='\0';
	//	puts(status_code);

		status_msg = malloc(5);
		memcpy(status_msg," OK\r\n",5);
		//status_msg[6]='\0';
		content_len = n-2;	
	}

	free(f_char);	

	connection = "Closed\r\n";
//	connection = malloc(9);
//	memcpy(connection,"Closed\r\n",8);
//	connection[8]='\0';
//
	//printf("Len %d\n",content_len);

	//printf("output being p\n\n");

/*	printf("%s\n",res);
	strcat(res,status_code);
	printf("%s\n",res);
	strcat(res,status_msg);
	//strcat(res, "HTTP/1.1 200 OK\r\n");
	printf("%s\n",res);
//	strcat(res,"Content-Length: 92\r\n");
*/


/*	sprintf(res,"%s%s%sContent-Length: %d",http_v,status_code,status_msg,content_len);
	printf("%s",res);
//	sprintf(res,"%d",content_len);
	strcat(res,"\r\n");
	strcat(res,"Connection: ");
	strcat(res,connection);
	strcat(res,"Content type: ");
	strcat(res,content_type);
	strcat(res,body);
*/
	//printf("%s",res);
	sprintf(res,"%s%s%sContent-Type: %sContent-Length: %d\r\nConnection: %s\r\n%s",http_v,status_code,status_msg,content_type,content_len,connection,body);

	free(http_v);
	free(status_code);
	free(status_msg);
//	free(connection);
	free(file_path);
	free(file);
//	free(body);

	//printf("%s",res);
	return res;

}


int main(int argc, char *argv[])
{
	
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

		
	if(kevent(kq,change_event,1,NULL,0,NULL)== -1){
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
	//	printf("%d new_ev\n", new_ev);
		for(int i = 0; new_ev > i; i++){

			//printf("%d",i);
			int eventfd = event[i].ident;
			
			if(event[i].flags & EV_EOF){
				printf("Client has disconnected\n\n");
				close(eventfd);
			}
			
			if(eventfd == sockfd_tcp){
				
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
				continue;
			}
		
			else if(event[i].filter & EVFILT_READ){
				bzero(buffer,500);
				n = recv(eventfd,buffer,sizeof(buffer),0);
				if(n <0){
					perror("ERROR in reading message\n");
					exit(1);
				}
				printf("%s",buffer);

				
				EV_SET(change_event, event[i].ident,EVFILT_READ, EV_DELETE,0,0,NULL);
				kevent(kq,change_event,1,NULL,0,NULL);

				EV_SET(change_event,eventfd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				kevent(kq,change_event,1,NULL,0,NULL);

			
				httpHeader = parse_request(buffer);

				n = write(eventfd,httpHeader,strlen(httpHeader));
				if(n<0){
					perror("ERROR writing to client\n");
					exit(1);
				}
				printf("Message sent is:\n%s\n",httpHeader);
				

				EV_SET(change_event, newsocket, EVFILT_WRITE,EV_DELETE,0,0,NULL);
				kevent(kq, change_event,1,NULL,0,NULL);
		
				

			}		

		}

	}
	return 0;
}
