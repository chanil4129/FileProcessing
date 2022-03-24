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
    int d_byte;
    int length;
    char buf[BUFFER_SIZE];
    char a_buf[BUFFER_SIZE];

    if(argc!=4){
	fprintf(stderr,"usage : %s <offset> <delete_byte> <file>\n",argv[0]);
	exit(1);
    }

    if((fd=open(argv[3],O_RDONLY))<0){
	fprintf(stderr,"open error for %s\n",argv[3]);
	exit(1);
    }

    offset=atoi(argv[1]);
    d_byte=atoi(argv[2]);

    if(d_byte<0) offset+=d_byte;
    length=read(fd,buf,offset);
    buf[length]=0;
    lseek(fd,(off_t)(offset+abs(d_byte)),SEEK_SET);
    length=read(fd,a_buf,BUFFER_SIZE);
    a_buf[length]=0;
    strcat(buf,a_buf);
    length=strlen(buf);

    close(fd);
    if((fd=open(argv[3],O_WRONLY|O_TRUNC))<0){
	fprintf(stderr,"reopen error for %s\n",argv[3]);
	exit(1);
    }
    write(fd,buf,length);
    close(fd);
    exit(0);
}
    



	


