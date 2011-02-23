#include "../SockServExe/StdAfx.h"


//arm-hismall-linux-g++ -o /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/UdpTest ./test/UdpTest.cpp -lpthread -lComuServ -L./ComuServ

#define  PACK_SIZE 1024

int main()
{
	int sock;
	BOOL blVal;
	int iVal;
	LINGER ling;
	struct sockaddr_in localaddr;
	struct sockaddr_in adrTo;
	unsigned long ulIP = INADDR_ANY;

	FILE *fp = NULL;
	char szBuf[PACK_SIZE+4] = {0};

	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( -1 == sock )
	{
		printf("create socket failed!\n");
		return 1;
	}
	
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(3031);
	localaddr.sin_addr.s_addr = ulIP;

	if( bind( sock, (SOCKADDR*)&localaddr, sizeof(localaddr) ) )
	{
		printf("bind socket failed!\n");
		return 1;
	}

	ling.l_onoff = 1; // 强制关闭且立即释放资源
	ling.l_linger = 0;
	setsockopt( sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling) );
	blVal = FALSE;
	setsockopt( sock, SOL_SOCKET, SO_OOBINLINE, (char*)&blVal, sizeof(blVal) );
	iVal = 1024*20;
	setsockopt( sock, SOL_SOCKET, SO_RCVBUF, (char*)&iVal, sizeof(iVal) );
	iVal = 0;
	setsockopt( sock, SOL_SOCKET, SO_SNDBUF, (char*)&iVal, sizeof(iVal) );

	// 设置为非阻塞
//	int iFlag = fcntl(sock, F_GETFL, 0);
//	fcntl(sock, F_SETFL, iFlag | O_NONBLOCK);

	adrTo.sin_family = AF_INET;
//	adrTo.sin_port = htons(10102);
//	adrTo.sin_addr.s_addr = inet_addr("202.118.226.74");
 	adrTo.sin_port = htons(4800);
	adrTo.sin_addr.s_addr = inet_addr("113.18.140.137");

	fp = fopen("test.264", "rb");
	if( NULL == fp )
	{
		printf("open file test.264 failed!\n");
		return 1;
	}

	int i=1;
	DWORD dwLastSendTm = GetTickCount();
	
	//fread(szBuf+4, 1, PACK_SIZE, fp);

_SEND:
//	while( GetTickCount() - dwLastSendTm > 50 )	
	for(i=1; i<=800; )
	{
//		for(int j=0; j<2; j++)
//		{
			if( PACK_SIZE == fread(szBuf+4, 1, PACK_SIZE, fp) )
			{
				memcpy(szBuf, (void*)&i, 4);

				if( 0 < sendto( sock, szBuf, PACK_SIZE+4, 0, (struct sockaddr*)&adrTo, sizeof(adrTo) ) )
				{
					printf("send %d pack, TickCount: %d\n", i++, GetTickCount());
				}
				else
				{
					printf("send failed!\n");
					perror("");
					break;
				}
			}
			else
			{
				break;
			}

//			i++;
//		}
	
//		dwLastSendTm = GetTickCount();

//		usleep(60000);
	}

//	if( i<=200 ) goto _SEND; 

	PRTMSG(MSG_NOR, "send over!\n");
	fclose(fp);
}

