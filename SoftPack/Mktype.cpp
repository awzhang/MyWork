#include 	<stdio.h>
#include 	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/stat.h>

#define BUFLEN 100*1024

int main(int argc, char* argv[])
{
	int iRet = 0;
	
	FILE *hFile = NULL;

	char *p = NULL;
	char szBuf[BUFLEN] = {0};
	char szFilePath[256] = "./GlobalShare/ComuServExport.h";
	struct stat objFileStat = {0};
	
	if( stat(szFilePath, &objFileStat) )
	{
		printf("Stat ComuServExport.h Fail!\n");
		return -1;
	}
	
	if( objFileStat.st_size>BUFLEN )
	{
		printf("ComuServExport.h Too Big!\n");
		return -1;
	}
	
	hFile = fopen(szFilePath, "r+");
	if( hFile==NULL )
	{
		printf("Open ComuServExport.h Fail!\n");
		return -1;
	}
	
	fread(szBuf, BUFLEN, 1, hFile);
	
//更改设备型号
	p = strstr(szBuf, "#define VEHICLE_TYPE");
	if( p==NULL )
	{
		printf("Strstr ComuServExport.h Fail!\n");
		goto VEHICLE_END;
	}
	else if( strcmp(argv[1],"M")==0 )
	{
		memcpy(p, "#define VEHICLE_TYPE VEHICLE_M ", 31);
	}
	else if( strcmp(argv[1],"V8")==0 )
	{
		memcpy(p, "#define VEHICLE_TYPE VEHICLE_V8", 31);
	}
	else if( strcmp(argv[1],"M2")==0 )
	{
		memcpy(p, "#define VEHICLE_TYPE VEHICLE_M2", 31);
	}
//更改手机模块型号
	p = strstr(szBuf, "strcpy(m_szPhoneMod,");
	if( p==NULL )
	{
		printf("Strstr ComuServExport.h Fail!\n");
		goto VEHICLE_END;
	}
	else if( strcmp(argv[2],"LC6311")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"LC6311\");  ", 33);
	}
	else if( strcmp(argv[2],"SIM4222")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"SIM4222\"); ", 33);
	}
	else if( strcmp(argv[2],"DTM6211")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"DTM6211\"); ", 33);
	}
	else if( strcmp(argv[2],"MC703OLD")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"MC703OLD\");", 33);
	}
	else if( strcmp(argv[2],"MC703NEW")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"MC703NEW\");", 33);
	}
	else if( strcmp(argv[2],"EM770W")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"EM770W\");  ", 33);
	}
	else if( strcmp(argv[2],"BENQ")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"BENQ\");    ", 33);
	}
	else if( strcmp(argv[2],"SIMCOM")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"SIMCOM\");  ", 33);
	}
	else if( strcmp(argv[2],"SIM5218")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"SIM5218\"); ", 33);
	}
	else if( strcmp(argv[2],"MU203")==0 )
	{
		memcpy(p, "strcpy(m_szPhoneMod, \"MU203\");   ", 33);
	}
//更改设备型号
	p = strstr(szBuf, "#define SMALL_VER");
	if( p==NULL )
	{
		printf("Strstr ComuServExport.h Fail!\n");
		goto VEHICLE_END;
	}
	else if( strcmp(argv[3],"0")==0 )
	{
		memcpy(p, "#define SMALL_VER  0", 20);
	}
	else if( strcmp(argv[3],"1")==0 )
	{
		memcpy(p, "#define SMALL_VER  1", 20);
	}
	
	if( fseek(hFile, 0, SEEK_SET) )
	{
		printf("Fseek ComuServExport.h Fail!\n");
		goto VEHICLE_END;
	}
	
	if( fwrite(szBuf, objFileStat.st_size, 1, hFile)!=1 )
	{
		printf("Write ComuServExport.h Fail!\n");
		goto VEHICLE_END;
	}

VEHICLE_END:
	fclose(hFile);
}
