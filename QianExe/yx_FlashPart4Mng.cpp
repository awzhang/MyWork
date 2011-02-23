#include "yx_QianStdAfx.h"
#include 	<sys/vfs.h>

#define DELOLD_PERTIME		4
#define PART4_MAX_USAGE		2.5*1024*1024		// 最大文件总和大小2.5M

#undef MSG_HEAD
#define MSG_HEAD	("QianExe-FlashMng")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlashPart4Mng::CFlashPart4Mng()
{
	
}

CFlashPart4Mng::~CFlashPart4Mng()
{
	
}

void CFlashPart4Mng::Init()
{
//	ChkFlash();
	DelOldFile();
}

void CFlashPart4Mng::DelOldFile()
{
	struct statfs objSta;
	LINUX_HANDLE hFind = {0};
	LINUX_FIND_DATA fd;
	double dUsage = 0;
	DWORD dwTotalSize = 0;
	char szOldestFile[MAX_PATH] = {0};
	char szFileName[MAX_PATH] = {0};

	for(int i=0; i<DELOLD_PERTIME; i++)
	{
		// 获取所有文件的大小总和
		dwTotalSize = GetToTalFileSize();
		PRTMSG(MSG_DBG, "part4 total file size: %d\n", dwTotalSize);
		if( dwTotalSize < PART4_MAX_USAGE )
		{
			PRTMSG(MSG_DBG, "Don not need to Delete file!\n");
			break;
		}
		
		// 查找最旧文件
		memset(szOldestFile, 0, sizeof(szOldestFile));
		sprintf(szOldestFile, "xxx_5012_8.dat");

		hFind = FindFirstFile( FLASH_PART4_PATH, "*.*", &fd, false);
		if( hFind.bValid )
		{
			for(;;)
			{
				// 文件名的前四个字符
				memset(szFileName, 0, sizeof(szFileName));
				memcpy(szFileName, fd.szFileName, 4);

				if( 0 != strcmp(".", fd.szFileName)						// 本级目录
					&& 0 != strcmp("..", fd.szFileName)					// 上级目录
					&& 0 != strcmp("DrvJg2", fd.szFileName)				// 旧的行驶记录仪的文件
					&& 0 != strcmp(DRVRECSTA_FILENAME, fd.szFileName)	// 行驶状态记录文件
					&& 0 != strcmp("HandsetData.dat", fd.szFileName)	// 手柄配置文件
					&& 0 != strcmp("Blk_", szFileName)					// 黑匣子文件
					&& 0 != strcmp("Acd_", szFileName)					// 事故疑点文件
					)
				{
					memset(szFileName, 0, sizeof(szFileName));
					sprintf(szFileName, "rm -f -r %s%s/", FLASH_PART4_PATH, fd.szFileName);
					
					if( !system(szFileName) )
					{
						PRTMSG(MSG_NOR, "Delete Part4 file: %s succ!\n", szFileName);
					}
					else
					{
						PRTMSG(MSG_NOR, "Delete Part4 file: %s failed!\n", szFileName);
						perror("");
					}

					goto _NEXT_FILE;
				}
				
				// 若是黑匣子文件或者事故疑点文件
				if( !strcmp("Blk_", szFileName) || !strcmp("Acd_", szFileName)) 
				{
					if( strlen(fd.szFileName) != 14 ||	// 若出现文件名长度不符合的文件
						strcmp(".dat", fd.szFileName + strlen(fd.szFileName) - 4 ))		// 若文件名后缀不符
					{
						// 删除文件然后跳过	
						memset(szFileName, 0, sizeof(szFileName));
						sprintf(szFileName, "%s%s", FLASH_PART4_PATH, fd.szFileName);
						unlink(szFileName);
						PRTMSG(MSG_NOR, "Delete Part4 file: %s\n", szFileName);
						
						continue;
					}
					else
					{
						if( strcmp(fd.szFileName + 4, szOldestFile + 4) < 0 ) // 若当前文件更旧
						{
							memset(szOldestFile, 0, sizeof(szOldestFile));
							memcpy(szOldestFile, fd.szFileName, sizeof(fd.szFileName));
						}
					}
				}
_NEXT_FILE:
				if( !FindNextFile(&hFind, &fd, false) )
				{
					//PRTMSG(MSG_DBG, "Find no more file!\n");
					break;
				}
			} 

			memset(&hFind, 0, sizeof(LINUX_HANDLE));

			// 删除最旧文件
			memset(szFileName, 0, sizeof(szFileName));
			sprintf(szFileName, "%s%s", FLASH_PART4_PATH, szOldestFile);
			unlink(szFileName);
			PRTMSG(MSG_NOR, "Delete Part4 file: %s\n", szFileName);
		}
		else
		{
			PRTMSG(MSG_DBG, "file:%s, hFind invalid!\n", fd.szFileName);
			break;
		}
	}

	memset(&hFind, 0, sizeof(LINUX_HANDLE));
}

DWORD CFlashPart4Mng::GetToTalFileSize()
{
	LINUX_FIND_DATA objFind;
	LINUX_HANDLE hFind = {0};
	DWORD dwTotalSize = 0;
	char  szFileName[MAX_PATH] = {0};
	
	FILE *fp = NULL;
	DWORD dwFileSize = 0;

	// 统计所有文件的大小
	hFind = FindFirstFile(FLASH_PART4_PATH, "*.*", &objFind, false);
	if( hFind.bValid )
	{
		for(;;)
		{
			memset(szFileName, 0, sizeof(szFileName));
			sprintf(szFileName, "%s%s", FLASH_PART4_PATH, objFind.szFileName);
			fp = fopen(szFileName, "rb");

			if( fp != NULL )
			{
				fseek(fp, 0, SEEK_END);
				dwFileSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				dwTotalSize += dwFileSize;
				//PRTMSG(MSG_DBG, "file name: %s, file size:%d\n", objFind.szFileName, dwFileSize);
				fclose(fp);
			}
			else
			{
				PRTMSG(MSG_DBG, "open file failed: %s\n", objFind.szFileName);
				perror("");
			}

			if( !FindNextFile(&hFind, &objFind, false) )
			{
				//PRTMSG(MSG_DBG, "Find no more file!\n");
				break;
			}
		}
		
		memset(&hFind, 0, sizeof(LINUX_HANDLE));
	}

	return dwTotalSize;
}

void CFlashPart4Mng::ChkFlash()
{
	LINUX_FIND_DATA objFind;
	LINUX_HANDLE hFind = {0};
	char szFileName[MAX_PATH] = {0};

	int iResult = 0;

	PRTMSG(MSG_DBG, "Flash part4 Checking..." );
	
	// 遍历文件名判断是否有不正常的文件,并且统计所有文件的大小
	hFind = FindFirstFile(FLASH_PART4_PATH, "*.*", &objFind, false);
	if( hFind.bValid )
	{
		do
		{
			if( S_IFDIR & objFind.objFileStat.st_mode )  // 若出现了子目录
			{
				iResult = 1;
				goto _ERASE_PART4;
			}

			// 旧的行驶记录文件
			if( !strcmp("DrvJg2", objFind.szFileName) ) continue;

			// 行驶状态记录文件
			if( !strcmp(DRVRECSTA_FILENAME, objFind.szFileName) ) continue;

			// 手柄配置文件
			if( !strcmp("HandsetData.dat", objFind.szFileName) ) continue;

			bool bInvalidFile = true;

			// { 事故疑点文件和黑匣子文件检查
			if( 14 != strlen(objFind.szFileName) ) // 若文件名长度不符
			{
				iResult = 3;
				goto _ERASE_PART4;
			}

			if( strcmp(".dat", objFind.szFileName + strlen(objFind.szFileName) - 4 ) ) // 若事故疑点文件名后缀不符
			{
				iResult = 4;
				goto _ERASE_PART4;
			}

			memset(szFileName, 0, sizeof(szFileName));
			memcpy(szFileName, objFind.szFileName, 4);
			if( !strcmp("Blk_", szFileName) ) // 若是黑匣子文件名开头
			{
				if( objFind.objFileStat.st_size > BLK_MAXSIZE ) // 若文件太大
				{
					iResult = 5;
					goto _ERASE_PART4;
				}
				else
					bInvalidFile = false;
			}

			if( !strcmp("Acd_", objFind.szFileName) ) // 若是事故疑点文件名开头
			{
				if( objFind.objFileStat.st_size > ACD_MAXSIZE ) // 若文件太大
				{
					iResult = 6;
					goto _ERASE_PART4;
				}
				else
					bInvalidFile = false;
			}
			// } 事故疑点文件和黑匣子文件检查


			if( bInvalidFile )
			{
				iResult = 7;
				goto _ERASE_PART4;
			}

		} while( FindNextFile(&hFind, &objFind, false) );
		
		memset(&hFind, 0, sizeof(LINUX_HANDLE));
	}

	PRTMSG(MSG_DBG, "Flash part4 Check Done, OK!\n" );

	return;

_ERASE_PART4:
	memset(&hFind, 0, sizeof(LINUX_HANDLE));

	PRTMSG(MSG_NOR, "Prepare erase Flash part4, because %d", iResult );

	ChangMtdRW("/dev/mtd/4");
	EraseMtd("/dev/mtd/4", NULL, 3*1024*1024);

	return;
}



