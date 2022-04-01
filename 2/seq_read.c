#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char **argv){
    struct timeval begin_t, end_t;
    int fd;
    char buf[200];
    long length;
    long offset;


    if(argc!=2){
	fprintf(stderr,"usage : %s <record_file_name>\n",argv[0]);
	exit(1);
    }
    if((fd=open(argv[1],O_RDONLY))<0){
	fprintf(stderr,"open error for %s\n",argv[1]);
	exit(1);
    }
    if((length=lseek(fd,0,SEEK_END))<0){
	fprintf(stderr,"lseek error\n");
	exit(1);
    }
    if(length%200!=0){
	fprintf(stderr,"data size isn't 200\n");
	exit(1);
    }
    lseek(fd,0,SEEK_SET);
    
    gettimeofday(&begin_t,NULL);
    for(offset=0;offset<length;offset+=200){
	read(fd,buf,200);
    }
    
    gettimeofday(&end_t,NULL);
    if(end_t.tv_usec < begin_t.tv_usec){
	end_t.tv_usec+=1000000;
    }
    end_t.tv_usec-=begin_t.tv_usec;
    printf("%ld usec\n",end_t.tv_usec);
    return 0;
}
