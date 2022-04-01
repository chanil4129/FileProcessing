#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define SUFFLE_NUM 10000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv){
    struct timeval begin_t,end_t;
    int *read_order_list;
    int num_of_records;
    int fd;
    char buf[200];
    long length;
    long offset;
    int i;

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
    num_of_records=length/200;
    read_order_list=(int*)malloc(sizeof(int)*num_of_records);
    GenRecordSequence(read_order_list, num_of_records);

    gettimeofday(&begin_t,NULL);
    for(i=0;i<num_of_records;i++){
	offset=read_order_list[i]*200;
	lseek(fd,offset,SEEK_SET);
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

void GenRecordSequence(int *list, int n){
    int i, j, k;

    srand((unsigned int)time(0));

    for(i=0; i<n; i++){
	list[i] = i;
    }

    for(i=0; i<SUFFLE_NUM; i++) {
	j = rand() % n;
	k = rand() % n;
	swap(&list[j], &list[k]);
    }
    return;
}

void swap(int *a, int *b){
    int tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;

    return;
}
