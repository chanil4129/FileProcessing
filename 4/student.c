#include <stdio.h>		// �ʿ��� header file �߰� ����
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "student.h"

//
// �Լ� readRecord()�� �л� ���ڵ� ���Ͽ��� �־��� rrn�� �ش��ϴ� ���ڵ带 �о 
// recordbuf�� �����ϰ�, ���� unpack() �Լ��� ȣ���Ͽ� �л� Ÿ���� ������ ���ڵ���
// �� �ʵ尪�� �����Ѵ�. �����ϸ� 1�� �׷��� ������ 0�� �����Ѵ�.
// unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� record���� �� field�� �����ϴ� ���� �Ѵ�.
//
int readRecord(FILE *fp, STUDENT *s, int rrn);
void unpack(const char *recordbuf, STUDENT *s);

//
// �Լ� writeRecord()�� �л� ���ڵ� ���Ͽ� �־��� rrn�� �ش��ϴ� ��ġ�� recordbuf�� 
// ����Ǿ� �ִ� ���ڵ带 �����Ѵ�. ������ pack() �Լ��� ȣ���Ͽ� recordbuf�� �����͸� ä�� �ִ´�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int writeRecord(FILE *fp, const STUDENT *s, int rrn);
void pack(char *recordbuf, const STUDENT *s);

//
// �Լ� appendRecord()�� �л� ���ڵ� ���Ͽ� ���ο� ���ڵ带 append�Ѵ�.
// ���ڵ� ���Ͽ� ���ڵ尡 �ϳ��� �������� �ʴ� ��� (ù ��° append)�� header ���ڵ带
// ���Ͽ� �����ϰ� ù ��° ���ڵ带 �����Ѵ�. 
// �翬�� ���ڵ带 append�� �� ������ header ���ڵ忡 ���� ������ �ڵ���� �Ѵ�.
// �Լ� appendRecord()�� ���������� writeRecord() �Լ��� ȣ���Ͽ� ���ڵ� ������ �ذ��Ѵ�.
// ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int appendRecord(FILE *fp, char *id, char *name, char *dept, char *addr, char *email);

//
// �л� ���ڵ� ���Ͽ��� �˻� Ű���� �����ϴ� ���ڵ尡 �����ϴ����� sequential search ����� 
// ���� ã�Ƴ���, �̸� �����ϴ� ��� ���ڵ��� ������ ����Ѵ�. �˻� Ű�� �л� ���ڵ带 �����ϴ�
// � �ʵ嵵 �����ϴ�. ���������� readRecord() �Լ��� ȣ���Ͽ� sequential search�� �����Ѵ�.
// �˻� ����� ����� �� �ݵ�� printRecord() �Լ��� ����Ѵ�. (�ݵ�� ���Ѿ� �ϸ�, �׷���
// �ʴ� ��� ä�� ���α׷����� �ڵ������� Ʋ�� ������ �ν���)
//
void searchRecord(FILE *fp, enum FIELD f, char *keyval);
void printRecord(const STUDENT *s);

//
// ���ڵ��� �ʵ���� enum FIELD Ÿ���� ������ ��ȯ���� �ش�.
// ���� ���, ����ڰ� ������ ���ɾ��� ���ڷ� "NAME"�̶�� �ʵ���� ����Ͽ��ٸ� 
// ���α׷� ���ο��� �̸� NAME(=1)���� ��ȯ�� �ʿ伺�� ������, �̶� �� �Լ��� �̿��Ѵ�.
//
enum FIELD getFieldID(char *fieldname);

void main(int argc, char *argv[]){
	FILE *fp;			// ��� file processing operation�� C library�� ����� ��
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
	//header ���� �ֱ�
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

	//����ü�� ������ ����
	strcpy(s.id,id);
	strcpy(s.name,name);
	strcpy(s.dept,dept);
	strcpy(s.addr,addr);
	strcpy(s.email,email);
		
	//record�� �ֱ�
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

