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

	/*char type[50];
	char date[50];
	char etag[50];
	char timeout[10];
	char max_timeout[10];
	char last_mod[50];
	char server[20];
	char mykey[50];
	char cookie_expires[50];
	char cookie_max_age[20];
	char cookie_path[100];
	char cookie_type[20];
	char trasfer_encod[20];
	char vary[50];
	char backend_server[100];
	char cache_info[50];
	char kuma_revision[50];
	char frame_options[20];
};*/

//typedef enum Method{UNSUPPORTED, GET, HEAD} Method; 

/*	char agent[100];
	char accept[50];
	char lang[50];
	char encod[50];
	char referer[100];
	char connec[20];
	char upgarde_insecure_request[100];
	char if_mod_since[100];
	char if_none_match[100];
	char cache_control[100];
};*/
/*
struct http_headers{
	struct requestHeader *reqHeader;
	struct resposeHeader *resHeader;
}http_headers;
*/
struct responseHeader{
	char *status_code;
	char *http_v;
	char *status_msg;
	char *content_type;
	char *body;
//	struct Header *headers;
//	char *server;
	int content_len;
	char *connection;

}responseHeader;

typedef struct requestHeader{
	char *method;
	char *file_path;
	char *file;
	char *http_v;
	struct Header *headers;
	char *body;
	struct responseHeader *res;
}requestHeader;

typedef struct Header{
	char *name;
	char *value;
	struct Header *next;
}Header;

//struct http_headers *http_header();
//struct responseHeader *create_response(const char *msg);
struct requestHeader *parse_request(const char *raw);
void free_header(struct Header *h);
void free_request(struct requestHeader *req);
void free_response(struct responseHeader *res);

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



struct requestHeader *parse_request(const char *raw){
	FILE *fptr;
	char *f_char;
	struct requestHeader *req = NULL;
	req = malloc(sizeof(requestHeader));
	if(!req)
		return NULL;
	memset(req,0,sizeof(requestHeader));
	
	//printf("req created\n");

	struct responseHeader *res = NULL;
	res = malloc(sizeof(responseHeader));
	if(!res)
		return NULL;
	memset(res,0,sizeof(responseHeader));
	
	
	//http method
	size_t method_len= strcspn(raw," ");
	req->method = malloc(method_len+1);
	if(!req->method){
		free_request(req);
		return NULL;
	}
	if(memcmp(raw, "GET", strlen("GET"))==0)
		req->method = "GET";
	else if(memcmp(raw, "HEAD", strlen("HEAD"))==0)
		req->method = "HEAD";
	else
		req->method = "UNSUPPORTED";
	raw += method_len+1;

	
	//http request url
	size_t file_len = strcspn(raw, " ");
	
	req->file_path = malloc(file_len+1);
	if(!req->file_path){
		free_request(req);
		return NULL;
	}
	memcpy(req->file_path,raw,file_len);
	//req->file_path[file_len]='\0';
	raw += 1;


	req->file = malloc(file_len);
	//printf("file mem\n");

	if(!req->file){
		free_request(req);
		return NULL;
	}
	memcpy(req->file,raw,file_len-1);
	req->file[file_len-1]='\0';
	raw += file_len ;
	printf("file %s\n",req->file);

	// content of requested file 
	res->status_code = malloc(4);
	if(!res->status_code){
		free_response(res);
		return NULL;
	}res->status_msg = malloc(20);
	if(!res->status_msg){
		free_response(res);
		return NULL;
	}
	res->connection = malloc(20);
	if(!res->connection){
		free_response(res);
		return NULL;
	}
//	printf("file path:%s\n",req->file_path);
	fptr = fopen(req->file,"r+");
	if(fptr == NULL){
		res->body = malloc(30);
		res->body = "Cannot open file";
		printf("Cannot open file\n");
		res->status_code = "404";
		res->status_msg = "Not Found";
		res->content_len = strlen("Cannot open file");
		
		//return NULL;
	
	//printf("%s%s",res->status_code,res->status_msg);
	}

	f_char = malloc(2);
	
	char *type;

	//type = req_file;
	//char file_type[10];
	//size_t type_len = strcspn(req->file,".");
	//type = malloc(10);

	type = strchr(req->file_path,'.');
	printf("209");	
	res->content_type = malloc(strlen(type)+1);
	if(!res->content_type){
		free_response(res);
		return NULL;
	}
	printf("here");
	//memcpy(file_type,req->file,type_len);
	//strcat(res->contetn_type,"")
	if(strcmp(type,".html")==0)
		res->content_type = "text/html";
	int n = 0;
	if(fptr != NULL){
	n=2;
	res->body = malloc(n);
	do{
		f_char[0]=fgetc(fptr);
		if(feof(fptr))
			break;
		n+=1;
		res->body = realloc(res->body,n);
		strcat(res->body,f_char);
	}
	while(1);
	
	//printf("%s",res->body);
	res->status_code = "200";
	res->status_msg = "OK";
	res->content_len = n;	
	}
	res->connection = "Closed";
	//printf("%s%s",res->status_code,res->status_msg);

	//struct stat filestat;
	//stat(req->file,&filestat);
	//printf("File modified",)
	
	//http request version
	size_t ver_len = strcspn(raw,EOL);
	req->http_v = malloc(ver_len+1);
	//printf("version mem\n");

	if(!req->http_v){
		free_request(req);
		return NULL;
	}
	memcpy(req->http_v,raw,ver_len);
	req->http_v[ver_len]='\0';
	raw += ver_len +2;
	
	//http response version
	res->http_v = malloc(ver_len+1);
	if(!res->http_v){
		free_response(res);
		return NULL;
	}
	memcpy(res->http_v,req->http_v,ver_len);
	res->http_v[ver_len]='\0';
	

	//http fields
	struct Header *header = NULL , *res_header = NULL, *last=NULL, *res_last= NULL;
/*	res_last = res_header;
	res_header = malloc(sizeof(Header));
	if(!res_header){
		res->headers = res_last;
		free_request(res);
		return NULL;
	}
	//response header field name
	res_header->name = malloc(name_len + 1);
	if(!res_header->name){
		free(res_header);
		res->res_headers = res_last;
		free_request(res);
		return NULL;
	}
	memcpy(res_header->name,raw,name_len);
	res_header[name_len] = '\0';

	//response field value
	res_header->value = malloc(value_len + 1);
	if(!res_header->value){
		free(res_header);
		res->res_headers = res_last;
		free_request(res);
		return NULL;
	}
	//printf("value mem\n");
	memcpy(res_header->value,raw,value_len);
	res_header->value[value_len]='\0';
	*/

	while(raw[0] != '\r' || raw[1] != '\n' ){
	
		last = header;
		header = malloc(sizeof(Header));
		if(!header){
			req->headers = last;
			free_request(req);
			return NULL;
		}
		//printf("header mem\n");

		
		//request field name
		//raw += 2;
		size_t name_len = strcspn(raw,":");
		header->name = malloc(name_len + 1);
		if(!header->name){
			free(header);
			req->headers = last;
			free_request(req);
			return NULL;
		}
		//printf("name mem\n");

		memcpy(header->name,raw,name_len);
		header->name[name_len]='\0';
		raw += name_len + 1;
		while(*raw == ' ')
			raw++;
		


		//request field value
		size_t value_len = strcspn(raw,EOL);
		header->value = malloc(value_len + 1);
		if(!header->value){
			free(header);
			req->headers = last;
			free_request(req);
			return NULL;
		}
		//printf("value mem\n");

		memcpy(header->value,raw,value_len);
		header->value[value_len]='\0';
		raw += value_len+2;
		

		//pointer to next field
		header->next = last;
		
	}
	req->headers = header;
	raw += 2;
	//res->headers = res_header;


	//http request body
	size_t body_len = strlen(raw);
	req->body = malloc(body_len + 1);
	if(!req->body){
		free_request(req);
		return NULL;
	}
	//printf("body mem\n");

	memcpy(req->body,raw,body_len);
	//req->body[body_len]="\0";
	
	
	
	req->res = res;
	
	return req;
}

/*
		return NULL;
	}
	//printf("body mem\n");

	memcpy(req->body,raw,body_len);
	//req->body[body_len]="\0";
	
	
	
	req->res = res;
	
	return req;
}


struct http_headers *htpp_header(const char *buffer){
	struct requestHeader *req = parse_request()
}*/

void free_response(struct responseHeader *res){
	if(res){
		free(res->http_v);
		free(res->status_code);
		free(res->status_msg);
		free(res->content_type);
		free(res->connection);
		free(res->body);
		free(res);
	}
}
void free_header(struct Header *h){
	if(h){
		free(h->name);
		free(h->value);
		free_header(h->next);
		free(h);
	}
}

void free_request(struct requestHeader *req){
	free(req->file);
	free(req->file_path);
	free(req->http_v);
	free_header(req->headers);
	free_response(req->res);
	free(req->body);
	free(req);
}
/*
void setHttpHeader(char httpHeader[])
{
	FILE *htmlData = fopen("index.html","r");
	char line[100];
	char responseData[8000] = "hello";
	while (fgets(line, 100, htmlData) != 0){
		strcat(responseData, line);
	}
	strcat(httpHeader, responseData);
}
*/

int main(int argc, char *argv[])
{
	struct requestHeader req_header;
	struct responseHeader res_keader;
	int sockfd_tcp,sockfd_udp,portno,newsocket,clilen,kq,new_ev;
	char buffer[1000];
	char httpHeader[8000]; 
	//	= "HTTP/1.1 200 OK\r\nHello";
	//Content-Type: text/plain\nContent-Length:12\n\nHello world!";
	struct sockaddr_in server_addr, cli_addr;
	int n;
	struct kevent event[5],change_event[5];
/*	if(argc < 2){
		perror("Port number was not entered\n");
		exit(1);
	}
*/	
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
	//printf("Socket created\n");
	bzero((char *) &server_addr, sizeof(server_addr));
//	portno = atoi( argv[1]);
	portno = 7010;
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

	//EV_SET(change_event, sockfd_udp, EVFILT_READ, EV_ADD|EV_ENABLE,0,0,0);
	
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
		//printf("%d new_ev\n", new_ev);
		for(int i = 0; new_ev > i; i++){
			int eventfd = event[i].ident;
			//printf("Entered into for\n");
			if(event[i].flags & EV_EOF){
				printf("Client has disconnectesd\n\n");
				close(eventfd);
			}
			
			else if(eventfd == sockfd_tcp){
				//setHttpHeader(httpHeader);
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
				struct requestHeader *req = parse_request(buffer);
				if(req){
					puts("Message body:\n");
					printf("%s %s %s\n",req->method,req->file_path,req->http_v);
					struct Header *h;
					for(h=req->headers;h;h=h->next){
					printf("%s: %s\n",h->name,h->value);
					}
					puts("Message body:\n");
					puts(req->body);
					struct responseHeader *res;
					res = req->res;
				//	stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(httpHeader,res->http_v)," "),res->status_code)," "),res->status_msg),EOL),res->body);
					sprintf(httpHeader,"%s %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n%s",res->http_v,res->status_code,res->status_msg,res->content_type,res->content_len,res->connection,res->body);
				}
			


				n = write(newsocket,httpHeader,sizeof(httpHeader));
				if(n<0){
					perror("ERROR writing to client\n");
					exit(1);
				}
				printf("Message sent is:%s\n",httpHeader);
				free_request(req);	
			}
		}
	}
	return 0;
}
