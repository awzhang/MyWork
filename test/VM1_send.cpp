#include <stdio.h>

#define  PACK_SIZE 1024

int main()
{
	int iComPort = -1;
	struct termios lOptions; 

	iComPort = open("/dev/usb/tts/0", O_RDWR);
	if( iComPort < 0 )
	{
		perror("open com failed");
		return 1;
	}
	
	tcgetattr(iComPort, &lOptions);
	lOptions.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	lOptions.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	lOptions.c_oflag &= ~OPOST;
	tcsetattr(iComPort, TCSANOW, &lOptions);

	FILE *fp = NULL;
	char szBuf[PACK_SIZE+4] = {0};

	fp = fopen("test.264", "rb");
	if( NULL == fp )
	{
		printf("open file test.264 failed!\n");
		return 1;
	}
	
	int i=1;
	DWORD dwLastSendTm = GetTickCount();
	
_SEND:
	while( GetTickCount() - dwLastSendTm > 50 )	
//	for(i=1; i<=200; )
	{
		if( PACK_SIZE == fread(szBuf+4, 1, PACK_SIZE, fp) )
		{
			memcpy(szBuf, (void*)&i, 4);
			
			if( 0 < write( iComPort, szBuf, PACK_SIZE+4 ) )
			{
				//printf("send %d pack, TickCount: %d\n", i++, GetTickCount());
				printf("send %d pack\n", i++);
			}
			else
			{
				printf("write failed!\n");
				perror("");
				break;
			}
		}
		else
		{
			break;
		}
		
		dwLastSendTm = GetTickCount();
		
//		usleep(60000);
	}
	
	if( i<=200 ) goto _SEND; 
	
	printf("send over!\n");
	fclose(fp);
	close(iComPort);

	return 0;
}