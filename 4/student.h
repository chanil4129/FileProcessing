#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE	85	// id(8) + name(10) + department(12) + address(30) + email(20) + 5*delimiter
#define HEADER_SIZE	8	// #records(4 bytes) + reserved(4 bytes)

// 필요한 경우 'define'을 추가할 수 있음.
#define BUFMAX 1024
#define IL 8
#define NL 10
#define DL 12
#define AL 30
#define EL 20

enum FIELD {ID=0, NAME, DEPT, ADDR, EMAIL};

typedef struct _STUDENT
{
	char id[9];		// 학번
	char name[11];		// 이름
	char dept[13];		// 학과
	char addr[31];		// 주소
	char email[21];		// 이메일 주소
} STUDENT;

#endif
