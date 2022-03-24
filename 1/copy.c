#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    char buf[BUFFER_SIZE];
    int fd1,fd2;
    int length;

    if(argc!=3){
	fprintf(stderr,"usage : %s <origin file> <clone file>\n",argv[0]);
	exit(1);
    }

    if((fd1=open(argv[1],O_RDONLY))<0){
	fprintf(stderr, "open error for %s\n",argv[1]);
	exit(1);
    }

    if((fd2=open(argv[2],O_WRONLY|O_CREAT,0664))<0){
	fprintf(stderr,"open error for %s\n",argv[2]);
	exit(1);
    }

    while((length=read(fd1,buf,10))>0)
	write(fd2,buf,length);
    
    close(fd1);
    close(fd2);
    exit(0);
}

