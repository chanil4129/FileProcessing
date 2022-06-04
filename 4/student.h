#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE	85	// id(8) + name(10) + department(12) + address(30) + email(20) + 5*delimiter
#define HEADER_SIZE	8	// #records(4 bytes) + reserved(4 bytes)

// �ʿ��� ��� 'define'�� �߰��� �� ����.
#define BUFMAX 1024
#define IL 8
#define NL 10
#define DL 12
#define AL 30
#define EL 20

enum FIELD {ID=0, NAME, DEPT, ADDR, EMAIL};

typedef struct _STUDENT
{
	char id[9];		// �й�
	char name[11];		// �̸�
	char dept[13];		// �а�
	char addr[31];		// �ּ�
	char email[21];		// �̸��� �ּ�
} STUDENT;

#endif
