#define PRINT_FOR_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "blkmap.h"

AddrMapTbl addrmaptbl;
extern FILE *devicefp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void print_block(int pbn);
void print_addrmaptbl_info();

//
// flash memory�� ó�� ����� �� �ʿ��� �ʱ�ȭ �۾�, ���� ��� address mapping table�� ����
// �ʱ�ȭ ���� �۾��� �����Ѵ�
//
void ftl_open()
{
	int i;

	// initialize the address mapping table
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		addrmaptbl.pbn[i] = -1;
	}

	//
	// �߰������� �ʿ��� �۾��� ������ �����ϸ� �ǰ� ������ ���ص� ������
	//

	return;
}

//
// file system�� ���� FTL�� �����ϴ� write interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
//
void ftl_write(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	print_addrmaptbl_info();
#endif

	//
	// block mapping ������� overwrite�� �߻��ϸ� �̸� �ذ��ϱ� ���� �ݵ�� �ϳ��� empty block��
	// �ʿ��ϸ�, �ʱⰪ�� flash memory���� �� ������ block number�� �����
	// overwrite�� �ذ��ϰ� �� �� �翬�� reserved_empty_blk�� overwrite�� ���߽�Ų (invalid) block�� �Ǿ�� ��
	// ���� reserved_empty_blk�� �����Ǿ� �ִ� ���� �ƴ϶� ��Ȳ�� ���� ��� �ٲ� �� ����
	//
	int reserved_empty_blk = DATABLKS_PER_DEVICE;
	char tempbuf[PAGE_SIZE];
	int lbn=lsn/4;
	int offset=lsn%4;
	int i;
	int pbn;
	int ppn;

	//logical, physical mapping
	for(i=0;i<DATABLKS_PER_DEVICE;i++){
		if(addrmaptbl.pbn[i]==lbn)
			break;
	}
	if(i==DATABLKS_PER_DEVICE){
		i=0;
		while(i<DATABLKS_PER_DEVICE&&addrmaptbl.pbn[i]!=-1)
			i++;
		if(i==DATABLKS_PER_DEVICE){
			fprintf(stderr,"space lack\n");
			return;
		}
	}
	pbn=i;
	ppn=pbn*4+offset;
	addrmaptbl.pbn[pbn]=lbn;

	//out-of-place update
	if(dd_read(ppn,tempbuf)){
		while(addrmaptbl.pbn[reserved_empty_blk]==-2)
			reserved_empty_blk--;
		for(i=0;i<4;i++){
			if(offset==i)
				break;
			if(dd_read(pbn*4+i,tempbuf))
				dd_write(reserved_empty_blk+i,tempbuf);
		}
		dd_write(reserved_empty_blk+offset,sectorbuf);
		dd_erase(pbn);
		for(i=0;i<4;i++){
			if(dd_read(reserved_empty_blk+i,tempbuf))
				dd_write(pbn*4+i,tempbuf);
		}
		addrmaptbl.pbn[reserved_empty_blk]=-2;
	}
	else
		dd_write(ppn,sectorbuf);
	return;
}

//
// file system�� ���� FTL�� �����ϴ� read interface
// 'sectorbuf'�� ����Ű�� �޸��� ũ��� 'SECTOR_SIZE'�̸�, ȣ���ϴ� �ʿ��� �̸� �޸𸮸� �Ҵ�޾ƾ� ��
// 
void ftl_read(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// �ʿ� �� ������ block mapping table�� ����� �� �� ����
	print_addrmaptbl_info();
#endif

	int lbn=lsn/4;
	int offset=lsn%4;
	int i;
	int pbn;
	int ppn;

	for(i=0;i<DATABLKS_PER_DEVICE;i++){
		if(addrmaptbl.pbn[i]==lbn)
			break;
	}
	if(i==DATABLKS_PER_DEVICE){
         i=0;
         while(addrmaptbl.pbn[i]!=-1||i!=DATABLKS_PER_DEVICE)
             i++;
         fprintf(stderr,"can't read\n");
     }   
	pbn=i;
	ppn=pbn*4+offset;
	dd_read(ppn,sectorbuf);

	return;
}

//
// for debugging
//
void print_block(int pbn)
{
	char *pagebuf;
	SpareData *sdata;
	int i;
	
	pagebuf = (char *)malloc(PAGE_SIZE);
	sdata = (SpareData *)malloc(SPARE_SIZE);

	printf("Physical Block Number: %d\n", pbn);

	for(i = pbn*PAGES_PER_BLOCK; i < (pbn+1)*PAGES_PER_BLOCK; i++)
	{
		read(i, pagebuf);
		memcpy(sdata, pagebuf+SECTOR_SIZE, SPARE_SIZE);
		printf("\t   %5d-[%7d]\n", i, sdata->lsn);
	}

	free(pagebuf);
	free(sdata);

	return;
}

//
// for debugging
//
void print_addrmaptbl_info()
{
	int i;

	printf("Address Mapping Table: \n");
	for(i = 0; i < DATABLKS_PER_DEVICE; i++)
	{
		if(addrmaptbl.pbn[i] >= 0)
		{
			printf("[%d %d]\n", i, addrmaptbl.pbn[i]);
		}
	}
}
