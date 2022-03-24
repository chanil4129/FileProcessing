#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024
 
int main(int argc, char *argv[]){
    int fd;
    int offset;
    int length;
    char a_buf[BUFFER_SIZE];
    char buf[BUFFER_SIZE];

    if(argc!=4){
	fprintf(stderr,"usage : %s <offset> <data> <file>\n",argv[0]);
	exit(1);
    }

    if((fd=open(argv[3],O_RDWR))<0){
	fprintf(stderr,"open error for %s\n",argv[3]);
	exit(1);
    }

    offset=atoi(argv[1]);
    strcpy(a_buf,argv[2]);
    strlen(a_buf);
    lseek(fd,(off_t)offset+1,SEEK_SET);
    length=read(fd,buf,BUFFER_SIZE);
    buf[length]=0;
    strcat(a_buf,buf);
    length=strlen(a_buf);

    lseek(fd,(off_t)offset+1,SEEK_SET);
    write(fd,a_buf,length);
    close(fd);
    exit(0);
}
