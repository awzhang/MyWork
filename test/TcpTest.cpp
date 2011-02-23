#include "../SockServExe/StdAfx.h"


//arm-hismall-linux-g++ -o TcpTest TcpTest.cpp -lpthread -lComuServ -L../ComuServ
//cp -f ./TcpTest /work/Hi3511_VSSDK_V1.1.2.3/root-dbg/TcpTest

#define  PACK_SIZE 1024

int main()
{
	int sock;
	struct sockaddr_in adrTo;
	FILE *fp = NULL;
	char szBuf[PACK_SIZE] = {0};
	
	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( -1 == sock )
	{
		printf("create socket failed!\n");
		return 1;
	}

	fp = fopen("test.264", "rb");
	if( NULL == fp )
	{
		printf("open file test.264 failed!\n");
		return 1;
	}

	adrTo.sin_family = AF_INET;
	adrTo.sin_port = htons(9020);
	adrTo.sin_addr.s_addr = inet_addr("59.61.82.172");

	if(0 == connect( sock, (struct sockaddr *)&adrTo, sizeof(struct sockaddr) ) )
	{
		printf("connect succ!\n");

		for(int i=1; i<=200; i++)
		{			
			if( PACK_SIZE == fread(szBuf, 1, PACK_SIZE, fp) )
			{
				if( 0 < send(sock, szBuf, PACK_SIZE, 0) )
				{
					printf("send %d pack, TickCount: %d\n", i, GetTickCount());
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

			usleep(300000);
		}	
	}

	close(sock);

	PRTMSG(MSG_NOR, "send over!\n");
	fclose(fp);
}

