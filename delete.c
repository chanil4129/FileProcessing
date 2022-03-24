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
    char buf[BUFFER_SIZE];

    if(argc!=4){
	fprintf(stderr,"usage : %s <offset> <delete_byte> <file>\n",argv[0]);
	exit(1);
    }

    if((fd=open(argv[3],O_WRONLY))<0){
	fprintf(stderr,"open error for %s\n",argv[3]);
	exit(1);
    }

    

