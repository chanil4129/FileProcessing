#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct Student{
    char name[15];
    char age[5];
    char address[20];
    char info[160];
} Student;

int main(int argc, char **argv){
    Student student;
    int fd;
    int d_count;

    if(argc!=3){
	fprintf(stderr,"usage : %s <#records> <record_file_name>\n",argv[0]);
	exit(1);
    }
    if((fd=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644))<0){
	fprintf(stderr,"open error for %s\n",argv[2]);
	exit(1);
    }
    d_count=atoi(argv[1]);
    
    for(int i=0;i<d_count;i++){
	strcpy(student.name,"chan");
	strcpy(student.age,"24");
	strcpy(student.address,"Seoul");
	strcpy(student.info,"student");
	write(fd,(char *)&student,sizeof(student));
    }

    return 0;
}