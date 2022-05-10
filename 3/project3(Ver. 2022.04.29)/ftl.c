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
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다
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
	// 추가적으로 필요한 작업이 있으면 수행하면 되고 없으면 안해도 무방함
	//

	return;
}

//
// file system을 위한 FTL이 제공하는 write interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
//
void ftl_write(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_addrmaptbl_info();
#endif

	//
	// block mapping 기법에서 overwrite가 발생하면 이를 해결하기 위해 반드시 하나의 empty block이
	// 필요하며, 초기값은 flash memory에서 맨 마지막 block number를 사용함
	// overwrite를 해결하고 난 후 당연히 reserved_empty_blk는 overwrite를 유발시킨 (invalid) block이 되어야 함
	// 따라서 reserved_empty_blk는 고정되어 있는 것이 아니라 상황에 따라 계속 바뀔 수 있음
	//
	int reserved_empty_blk = DATABLKS_PER_DEVICE;
	int lbn=lsn/PAGES_PER_BLOCK;
	int offset=lsn%PAGES_PER_BLOCK;
	int pbn=addrmaptbl.pbn[lbn];
	int ppn;
	int i;
	char *pagebuf;
	SpareData *sdata;
	
	pagebuf=(char *)malloc(PAGE_SIZE);
	sdata=(SpareData *)malloc(SPARE_SIZE);

	//mapping
	i=0;
	if(pbn==-1){
		for(int j=0;j<DATABLKS_PER_DEVICE;j++){
			if(addrmaptbl.pbn[j]==i){
				i++;
				j=-1;
			}
		}
		addrmaptbl.pbn[lbn]=i;
		pbn=addrmaptbl.pbn[lbn];
	}
	ppn=pbn*PAGES_PER_BLOCK+offset;

	dd_read(pbn*PAGES_PER_BLOCK+offset,pagebuf);
	memcpy(sdata,pagebuf+SECTOR_SIZE,SPARE_SIZE);
	
	//write
	if(sdata->lsn==-1)
		dd_write(ppn,sectorbuf);
	//overwrite
	else{
		reserved_empty_blk--;
		for(int j=0;j<DATABLKS_PER_DEVICE;j++){
			if(addrmaptbl.pbn[j]==reserved_empty_blk){
				reserved_empty_blk--;
				j=-1;
			}
		}
				
		for(i=0;i<PAGES_PER_BLOCK;i++){
			if(offset==i)
				continue;
			dd_read(pbn*PAGES_PER_BLOCK+i,pagebuf);
			dd_write(reserved_empty_blk*PAGES_PER_BLOCK+i,pagebuf);
		}
		dd_write(reserved_empty_blk*PAGES_PER_BLOCK+offset,sectorbuf);
		dd_erase(pbn);
		addrmaptbl.pbn[lbn]=reserved_empty_blk;
	}
	
	sdata->lsn=lsn;
	memcpy(pagebuf+SECTOR_SIZE,sdata,SPARE_SIZE);

	free(pagebuf);
	free(sdata);

	return;
}

//
// file system을 위한 FTL이 제공하는 read interface
// 'sectorbuf'가 가리키는 메모리의 크기는 'SECTOR_SIZE'이며, 호출하는 쪽에서 미리 메모리를 할당받아야 함
// 
void ftl_read(int lsn, char *sectorbuf)
{
#ifdef PRINT_FOR_DEBUG			// 필요 시 현재의 block mapping table을 출력해 볼 수 있음
	print_addrmaptbl_info();
#endif

	int lbn=lsn/PAGES_PER_BLOCK;
	int offset=lsn%PAGES_PER_BLOCK;
	int pbn=addrmaptbl.pbn[lbn];
	int ppn=pbn*PAGES_PER_BLOCK+offset;

	if(pbn==-1){
		fprintf(stderr,"read error\n");
		return;
	}
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
		dd_read(i, pagebuf);
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
