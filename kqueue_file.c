#include<stdio.h>
#include<err.h>
#include<sys/event.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>

int main(int argc, char *argv[]){
	struct kevent event;
	struct kevent tevent;
	int kq,fd,ret;
	if (argc != 2)
		err(EXIT_FAILURE, "Usage: %s path\n",argv[1]);
	fd = open(argv[1],O_RDONLY);
	if(fd == -1)
		err(EXIT_FAILURE, "Failed to open %s",argv[1]);

	kq = kqueue();
	if(kq == -1)
		err(EXIT_FAILURE, "kqueue() failed");
	
	EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_WRITE,0,NULL);
	ret = kevent(kq, &event, 1, NULL, 0, NULL);
	if(ret == -1)
		err(EXIT_FAILURE, "kevent register");
	if(event.flags & EV_ERROR)
		err(EXIT_FAILURE, "Event error: %s:", strerror(event.data));
	for(;;){
	ret = kevent(kq,NULL,0,&tevent,1,NULL );
	if(ret == -1){
	err(EXIT_FAILURE, "kevent wait");
	}
	else if(ret>0)
		printf("Something was written in %s\n",argv[1]);

	}

}
