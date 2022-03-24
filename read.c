#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    int fd;
    int offset;
    int r_byte;
    char buf[BUFFER_SIZE];
    char c;
    int i=0;

    if(argc!=4){
	fprintf(stderr,"usage :%s <offset> <byte_size> <file> : ",argv[0]);
	exit(1);
    }

    if((fd=open(argv[3],O_RDONLY))<0){
	fprintf(stderr,"open error for %s\n",argv[3]);
	exit(1);
    }

    offset=atoi(argv[1]);
    r_byte=atoi(argv[2]);
    lseek(fd,(off_t)(offset+r_byte),SEEK_SET);
    r_byte=abs(r_byte);

    while(read(fd,&c,1)>0&&r_byte>0){
	buf[i++]=c;
	r_byte--;
    }
    buf[i]=0;
    printf("%s",buf);

    close(fd);
    exit(0);
}
