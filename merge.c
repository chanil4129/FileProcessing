#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    int fd1,fd2,fd3;
    char buf[BUFFER_SIZE];
    int length;

    if(argc!=4){
	fprintf(stderr,"usage : %s <file1> <file2> <merge file>\n", argv[0]);
	exit(1);
    }

    fd1=open(argv[1],O_RDONLY);
    fd2=open(argv[2],O_RDONLY);

    if(fd1<0||fd2<0){
	fprintf(stderr, "open error\n");
	exit(1);
    }

    if((fd3=open(argv[3],O_WRONLY|O_CREAT,0644))<0){
	fprintf(stderr,"open error for %s\n",argv[3]);
	exit(1);
    }

    while((length=read(fd1,buf,BUFFER_SIZE))>0){
	write(fd3,buf,length);
    }
    //lseek(fd3,(off_t)length,SEEK_SET);
    while((length=read(fd2,buf,BUFFER_SIZE))>0){
	write(fd3,buf,length);
    }
    close(fd1);
    close(fd2);
    close(fd3);
    exit(0);
}
    

