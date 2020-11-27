#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main(){
	FILE *fp;
	int c;
	char *cr;
	char *con;
	int n=2;
	cr = malloc(2);
	fp= fopen("http_server/index.html","r");
	if(fp==NULL){
		perror("Error in opening");
		return(-1);
	}
	con = malloc(n);
	do{
		cr[0]=fgetc(fp);
		if(feof(fp)){
			break;
		}
		n+=1;
		con = realloc(con,n);
		strcat(con,cr);
		//printf("%c",c);
	}while(1);
	printf("Content of file is\n%s",con);
	fclose(fp);
	return(0);
}
