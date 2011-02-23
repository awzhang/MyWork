#include 	<stdio.h>
#include 	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/stat.h>

int main(int argc, char* argv[])
{
	int i;
	int iRet = 0;
	
	FILE *hOFile = NULL;
	FILE *hIFile[8] = {NULL};
	
	struct stat objIFileStat[8] = {0};
	int iIFileNum;
	char szBufRead[4*1024] = {0xff};
	char szOFilePath[256];
	char szIFilePath[8][256];
	unsigned long dwStartPos[8], dwEndPos[8];
	unsigned long dwCount;
	
	if( strcmp(argv[1],"M")==0 )
	{
		mkdir( "../M_bin", 0 );

		iIFileNum = 8;
		strcpy(szOFilePath, "../M_bin/M_image.bin");
		
		strcpy(szIFilePath[0], "../M_bin/M_boot.bin");
		strcpy(szIFilePath[1], "../M_bin/M_env.bin");
		strcpy(szIFilePath[2], "../M_bin/M_kernel.bin");
		strcpy(szIFilePath[3], "../M_bin/M_root.bin");
		strcpy(szIFilePath[4], "../M_bin/M_update.bin");
		strcpy(szIFilePath[5], "../M_bin/M_black.bin");
		strcpy(szIFilePath[6], "../M_bin/M_app.bin");
		strcpy(szIFilePath[7], "../M_bin/M_config.bin");
		
		dwStartPos[0] = 0;
		dwStartPos[1] = 0x00040000;
		dwStartPos[2] = 0x00100000;
		dwStartPos[3] = 0x00300000;
		dwStartPos[4] = 0x01000000;
		dwStartPos[5] = 0x01500000;
		dwStartPos[6] = 0x01800000;
		dwStartPos[7] = 0x01f00000;
		
		dwEndPos[0] = 0x00040000;
		dwEndPos[1] = 0x00100000;
		dwEndPos[2] = 0x00300000;
		dwEndPos[3] = 0x01000000;
		dwEndPos[4] = 0x01500000;
		dwEndPos[5] = 0x01800000;
		dwEndPos[6] = 0x01f00000;
		dwEndPos[7] = 0x02000000;
	}
	else if( strcmp(argv[1],"V8")==0 )
	{
		mkdir( "../V8_bin", 0 );

		iIFileNum = 7;
		strcpy(szOFilePath, "../V8_bin/V8_image.bin");
		
		strcpy(szIFilePath[0], "../V8_bin/V8_boot.bin");
		strcpy(szIFilePath[1], "../V8_bin/V8_kernel.bin");
		strcpy(szIFilePath[2], "../V8_bin/V8_root.bin");
		strcpy(szIFilePath[3], "../V8_bin/V8_update.bin");
		strcpy(szIFilePath[4], "../V8_bin/V8_black.bin");
		strcpy(szIFilePath[5], "../V8_bin/V8_app.bin");
		strcpy(szIFilePath[6], "../V8_bin/V8_config.bin");

		dwStartPos[0] = 0;
		dwStartPos[1] = 0x00100000;
		dwStartPos[2] = 0x00300000;
		dwStartPos[3] = 0x01000000;
		dwStartPos[4] = 0x01500000;
		dwStartPos[5] = 0x01800000;
		dwStartPos[6] = 0x01f00000;
		
		dwEndPos[0] = 0x00100000;
		dwEndPos[1] = 0x00300000;
		dwEndPos[2] = 0x01000000;
		dwEndPos[3] = 0x01500000;
		dwEndPos[4] = 0x01800000;
		dwEndPos[5] = 0x01f00000;
		dwEndPos[6] = 0x02000000;
	}

	hOFile = fopen(szOFilePath, "w+b");
	if( hOFile==NULL )
	{
		printf("Open Output File %s Fail!\n", szOFilePath);
		goto MERGE_END;
	}
	
	for(i = 0; i < iIFileNum; i++)
	{
		if( access(szIFilePath[i], F_OK) )
		{
			printf("Input File %s Lack!\n", szIFilePath[i]);
			
			for(dwCount = 0; dwCount < (dwEndPos[i]-dwStartPos[i])/sizeof(szBufRead); dwCount++)
			{
				memset(szBufRead, 0xff, sizeof(szBufRead));
				fwrite(szBufRead, 1, sizeof(szBufRead), hOFile);
			}
		}
		else
		{
			stat(szIFilePath[i], &objIFileStat[i]);
			if( !objIFileStat[i].st_size )
			{
				printf("Input File %s Empty!\n", szIFilePath[i]);
				goto MERGE_END;
			}
		
			hIFile[i] = fopen(szIFilePath[i], "r+");
			if( hIFile[i]==NULL )
			{
				printf("Open Input File %s Fail!\n", szIFilePath[i]);
				goto MERGE_END;
			}
			
			for(dwCount = 0; dwCount < (dwEndPos[i]-dwStartPos[i])/sizeof(szBufRead); dwCount++)
			{
				memset(szBufRead, 0xff, sizeof(szBufRead));
				fread(szBufRead, 1, sizeof(szBufRead), hIFile[i]);
				fwrite(szBufRead, 1, sizeof(szBufRead), hOFile);
			}
		}
	}

MERGE_END:
	for(i=0; i<iIFileNum; i++)
	{
		if(hIFile[i])
			fclose(hIFile[i]);
	}
	
	if(hOFile)
		fclose(hOFile);
}
