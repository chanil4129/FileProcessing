#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc,char *argv[]){
    int fd;
    int offset;
    int a_length;
    char buf[BUFFER_SIZE];

    if(argc!=4){
	fprintf(stderr,"usage : %s <offset> <data> <file>\n",argv[0]);
	exit(1);
    }

    if((fd=open(argv[3],O_WRONLY))<0){
	fprintf(stderr, "open error for %s\n",argv[3]);
	exit(1);
    }

    offset=atoi(argv[1]);
    a_length=strlen(argv[2]);
    lseek(fd,(off_t)offset,SEEK_SET);

    write(fd,argv[2],a_length);
    close(fd);
    exit(0);
}
