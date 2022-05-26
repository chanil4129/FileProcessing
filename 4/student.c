#include <stdio.h>		// 필요한 header file 추가 가능
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "student.h"

//
// 함수 readRecord()는 학생 레코드 파일에서 주어진 rrn에 해당하는 레코드를 읽어서 
// recordbuf에 저장하고, 이후 unpack() 함수를 호출하여 학생 타입의 변수에 레코드의
// 각 필드값을 저장한다. 성공하면 1을 그렇지 않으면 0을 리턴한다.
// unpack() 함수는 recordbuf에 저장되어 있는 record에서 각 field를 추출하는 일을 한다.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// 함수 writeRecord()는 학생 레코드 파일에 주어진 rrn에 해당하는 위치에 recordbuf에 
// 저장되어 있는 레코드를 저장한다. 이전에 pack() 함수를 호출하여 recordbuf에 데이터를 채워 넣는다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// 함수 appendRecord()는 학생 레코드 파일에 새로운 레코드를 append한다.
// 레코드 파일에 레코드가 하나도 존재하지 않는 경우 (첫 번째 append)는 header 레코드를
// 파일에 생성하고 첫 번째 레코드를 저장한다. 
// 당연히 레코드를 append를 할 때마다 header 레코드에 대한 수정이 뒤따라야 한다.
// 함수 appendRecord()는 내부적으로 writeRecord() 함수를 호출하여 레코드 저장을 해결한다.
// 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// 학생 레코드 파일에서 검색 키값을 만족하는 레코드가 존재하는지를 sequential search 기법을 
// 통해 찾아내고, 이를 만족하는 모든 레코드의 내용을 출력한다. 검색 키는 학생 레코드를 구성하는
// 어떤 필드도 가능하다. 내부적으로 readRecord() 함수를 호출하여 sequential search를 수행한다.
// 검색 결과를 출력할 때 반드시 printRecord() 함수를 사용한다. (반드시 지켜야 하며, 그렇지
// 않는 경우 채점 프로그램에서 자동적으로 틀린 것으로 인식함)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// 레코드의 필드명을 enum FIELD 타입의 값으로 변환시켜 준다.
// 예를 들면, 사용자가 수행한 명령어의 인자로 "NAME"이라는 필드명을 사용하였다면 
// 프로그램 내부에서 이를 NAME(=1)으로 변환할 필요성이 있으며, 이때 이 함수를 이용한다.
//
enum FIELD getFieldID(char *fieldname);

void main(int argc, char *argv[]){
	FILE *fp;			// 모든 file processing operation은 C library를 사용할 것
	enum FIELD field;

	if(strcmp(argv[1],"-a")&&strcmp(argv[1],"-s")){
		fprintf(stderr,"usage: %s <option> ...\n",argv[0]);
		exit(1);
	}
	
	if(!strcmp(argv[1],"-a")&&argc!=8){
		fprintf(stderr,"usage: %s -a <record_file_name> \"field_value1\" \"field_value2\" \"field_value3\" \"field_value4\" \"field_value5\"\n",argv[0]);
		exit(1);
	}

	if(!strcmp(argv[1],"-s")&&argc!=4){
		fprintf(stderr,"usage: %s -s <record_file_name> \"field_name=field_value\"\n",argv[0]);
		exit(1);
	}

	if(!strcmp(argv[1],"-a")){
		if((fp=fopen(argv[2],"r+"))==NULL){
			if((fp=fopen(argv[2],"w+"))==NULL){
				fprintf(stderr,"fopen error\n");
				exit(1);
			}
		}
		if(!appendRecord(fp,argv[3],argv[4],argv[5],argv[6],argv[7])){
			printf("appendRecord error\n");
			exit(1);
		}
	}

	if(!strcmp(argv[1],"-s")){
		if((fp=fopen(argv[2],"r+"))==NULL){
			fprintf(stderr,"fopen error for %s\n",argv[2]);
			exit(1);
		}
		field=getFieldID(argv[3]);
		searchRecord(fp,field,argv[3]);
	}

	fclose(fp);
	exit(0);
}

int readRecord(FILE *fp, STUDENT *s, int rrn){
	char recordbuf[RECORD_SIZE];
	long offset;

	offset=rrn*RECORD_SIZE+HEADER_SIZE;
	if(fseek(fp,offset,SEEK_SET)!=0){
		fprintf(stderr,"seek error\n");
		return 0;
	}
	if(fread(recordbuf,sizeof(recordbuf),1,fp)!=1){
		fprintf(stderr,"read error\n");
		return 0;
	}
	unpack(recordbuf,s);
	return 1;
}

void unpack(const char *recordbuf, STUDENT *s){
	memcpy(s->id,recordbuf,IL);
	s->id[IL]='\0';
	memcpy(s->name,recordbuf+1+IL,NL);
	s->name[NL]='\0';
	memcpy(s->dept,recordbuf+2+IL+NL,DL);
	s->dept[DL]='\0';
	memcpy(s->addr,recordbuf+3+IL+NL+DL,AL);
	s->addr[AL]='\0';
	memcpy(s->email,recordbuf+4+IL+NL+DL+AL,EL);
	s->email[EL]='\0';
}

int writeRecord(FILE *fp, const STUDENT *s, int rrn){
	char recordbuf[RECORD_SIZE];
	long offset;

	pack(recordbuf,s);
	offset=HEADER_SIZE+rrn*RECORD_SIZE;
	if(fseek(fp,offset,SEEK_SET)!=0){
		fprintf(stderr,"seek error\n");
		return 0;
	}
	if(fwrite(recordbuf,sizeof(recordbuf),1,fp)!=1){
		fprintf(stderr,"write error\n");
		return 0;
	}

	return 1;
}

void pack(char *recordbuf, const STUDENT *s){
	memcpy(recordbuf,s->id,IL);
	memcpy(recordbuf+IL,"#",1);
	memcpy(recordbuf+IL+1,s->name,NL);
	memcpy(recordbuf+IL+NL+1,"#",1);
	memcpy(recordbuf+IL+NL+2,s->dept,DL);
	memcpy(recordbuf+IL+NL+DL+2,"#",1);
	memcpy(recordbuf+IL+NL+DL+3,s->addr,AL);
	memcpy(recordbuf+IL+NL+DL+AL+3,"#",1);
	memcpy(recordbuf+IL+NL+DL+AL+4,s->email,EL);
	memcpy(recordbuf+IL+NL+DL+AL+EL+4,"#",1);
}

int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email){
	STUDENT s;
	char headbuf[HEADER_SIZE];
	char recordbuf[RECORD_SIZE];
	int rrn;
	int fd;
	char *ptr=NULL;

	fd=fileno(fp);
	//header 정보 넣기
	if(lseek(fd,0,SEEK_END)==0){
		strcpy(headbuf,"1");
		rrn=0;
	}
	else {
		lseek(fd,0,SEEK_SET);
		read(fd,headbuf,HEADER_SIZE);
		rrn=atoi(headbuf);
		sprintf(headbuf,"%d",rrn+1);
	}
	lseek(fd,0,SEEK_SET);
	write(fd,headbuf,HEADER_SIZE);

	//구조체에 데이터 삽입
	strcpy(s.id,id);
	strcpy(s.name,name);
	strcpy(s.dept,dept);
	strcpy(s.addr,addr);
	strcpy(s.email,email);
		
	//record에 넣기
	if(!writeRecord(fp,&s,rrn)){
		printf("writeRecord error\n");
		return 0;
	}
	return 1;
}

void searchRecord(FILE *fp, enum FIELD f, char *keyval){
	STUDENT s;
	char recordbuf[RECORD_SIZE];
	int rrn=0;
	int eof;
	int fd;

	keyval=strstr(keyval,"=")+1;
	fd=fileno(fp);
	eof=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	for(int i=HEADER_SIZE;i<eof;i+=RECORD_SIZE){
		if(!readRecord(fp,&s,rrn)){
			printf("none data\n");
			return;
		}
		if(f==ID&&!strcmp(s.id,keyval))
			printRecord(&s);
		if(f==NAME&&!strcmp(s.name,keyval))
			printRecord(&s);
		if(f==DEPT&&!strcmp(s.dept,keyval))
			printRecord(&s);
		if(f==ADDR&&!strcmp(s.addr,keyval))
			printRecord(&s);
		if(f==EMAIL&&!strcmp(s.email,keyval))
			printRecord(&s);
		rrn++;
	}
}

enum FIELD getFieldID(char *fieldname){
	char *ptr=NULL;
	if((ptr=strstr(fieldname,"ID"))!=NULL)
		return ID;
	if((ptr=strstr(fieldname,"NAME"))!=NULL)
		return NAME;
	if((ptr=strstr(fieldname,"DEPT"))!=NULL)
		return DEPT;
	if((ptr=strstr(fieldname,"ADDR"))!=NULL)
		return ADDR;
	if((ptr=strstr(fieldname,"EMAIL"))!=NULL)
		return EMAIL;
}

void printRecord(const STUDENT *s){
	printf("%s | %s | %s | %s | %s\n", s->id, s->name, s->dept, s->addr, s->email);
}

