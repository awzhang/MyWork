#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILE_NUM	7

typedef unsigned int DWORD;
typedef unsigned char BYTE;

// 升级包的数据结构: 文件个数(1) + 【文件目录(40) + 文件名称(40) + 文件长度(4) + 文件数据(n)】*N + 校验和(4)

int main(int argc, char* argv[])
{
	int  i, j;
	int  iResult = 1;
	FILE *fpPack = NULL;
	FILE *fpSrc = NULL;
	char *szBuf = NULL;
	char szPackFileData;
	DWORD dwChkSum = 0;
	int  iPackFileSize = 0;
	int  iSrcFileSize = 0;
	char szPackFile[40];
	BYTE bytSrcFileCnt = FILE_NUM;
	char szSrcFileName[FILE_NUM][40] = {"QianExe", "ComuExe", "IOExe", "SockServExe", "DvrExe", "UpdateExe", "libComuServ.so"};
	char szSrcFilePath[FILE_NUM][40] = {"/mnt/Flash/part5/", "/mnt/Flash/part5/", "/mnt/Flash/part5/", "/mnt/Flash/part5/", "/mnt/Flash/part5/", "/mnt/Flash/part5/", "/mnt/Flash/part5/"};
	char szSrcFile[FILE_NUM][40] = {"./QianExe/QianExe", "./ComuExe/ComuExe", "./IOExe/IOExe", "./SockServExe/SockServExe", "./DvrExe/DvrExe", "./UpdateExe/UpdateExe", "./ComuServ/libComuServ.so"};
	
	if(strcmp(argv[1], "M") == 0)
	{
		mkdir( "../M_bin" , 0);
		strcpy(szPackFile, "../M_bin/M_exe.bin");
	}
	else if(strcmp(argv[1],"V8") == 0)
	{
		mkdir( "../V8_bin" , 0);
		strcpy(szPackFile, "../V8_bin/V8_exe.bin");
	}
	
	for(i = 0; i < bytSrcFileCnt; i++)
	{
		if(access(szSrcFile[i], F_OK) != 0)
		{
			printf("There is no %s!\n", szSrcFileName[i]);
			goto _PACK_END;
		}
	}

	// 创建打包文件
	if(NULL == (fpPack = fopen(szPackFile, "wb+")))
	{
		perror("Create exe.bin failed:");
		goto _PACK_END;
	}

	// 文件个数
	if(1 != fwrite((void*)&bytSrcFileCnt, 1, 1, fpPack))
	{
		perror("Write file num failed:");
		goto _PACK_END;
	}

	// 各个文件
	for(i = 0; i < bytSrcFileCnt; i++)
	{
		// 打开文件
		if(NULL == (fpSrc = fopen(szSrcFile[i], "rb+")))
		{
			perror("Open src file failed:");
			goto _PACK_END;
		}

		// 写文件目录
		if(40 != fwrite(szSrcFilePath[i], 1, 40, fpPack))
		{
			perror("Write src file path failed:");
			goto _PACK_END;
		}
		
		// 写文件名
		if(40 != fwrite(szSrcFileName[i], 1, 40, fpPack))
		{
			perror("Write src file name failed:");
			goto _PACK_END;
		}

		// 计算文件大小
		fseek(fpSrc, 0, SEEK_END);
		iSrcFileSize = ftell(fpSrc);

		// 写文件大小
		if(4 != fwrite((void*)&iSrcFileSize, 1, 4, fpPack))
		{
			perror("Write src file size failed:");
			goto _PACK_END;
		}
		
		// 分配缓冲
		if(NULL == (szBuf = (char*)malloc(iSrcFileSize)))
		{
			perror("Malloc src file buf failed:");
			goto _PACK_END;
		}

		// 读写文件数据
		fseek(fpSrc, 0, SEEK_SET);
		if(iSrcFileSize != fread(szBuf, 1, iSrcFileSize, fpSrc))
		{
			perror("Read src file data failed:");
			goto _PACK_END;
		}
		
		if(iSrcFileSize != fwrite(szBuf, 1, iSrcFileSize, fpPack))
		{
			perror("Write src file data failed:");
			goto _PACK_END;
		}

		if(fpSrc != NULL)	
		{
			fclose(fpSrc);
			fpSrc = NULL;
		}
		
		if(szBuf != NULL) 
		{	
			free(szBuf);
			szBuf = NULL;
		}		

		printf("Pack %s succ\n", szSrcFileName[i]);
	}
	
	iPackFileSize = ftell(fpPack);
	fseek(fpPack, 0, SEEK_SET);
	
	for(j = 0; j < iPackFileSize; j++)
	{
		if(1 != fread((void*)&szPackFileData, 1, 1, fpPack))
		{
			perror("Read pack file data failed:");
			goto _PACK_END;
		}
		
		dwChkSum += (BYTE)szPackFileData;
	}
	
	printf("ChkSum = %d\n", dwChkSum);
	
	// 写校验和
	if(4 != fwrite((void*)&dwChkSum, 1, 4, fpPack))
	{
		perror("Write chk sum failed:");
		goto _PACK_END;
	}

	iResult = 0;

_PACK_END:
	if( fpPack != NULL )
	{
		fclose(fpPack);
		fpPack = NULL;
	}

	if(iResult)
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Pack failed !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	else
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Pack Succ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	return iResult;
}
