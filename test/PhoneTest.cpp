#include "../SockServExe/StdAfx.h"

int main()
{
	int m_iPhonePort = 0;
	struct termios options;

	IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);

	system("insmod /dvs/extdrv/linux_usbserial.ko vendor=0x12d1 product=0x1001");

	sleep(5);

	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);
	sleep(3);
	IOSet(IOS_PHONERST, IO_PHONERST_LO, NULL, 0);
	sleep(1);
	IOSet(IOS_PHONERST, IO_PHONERST_HI, NULL, 0);
	sleep(3);
	PRTMSG(MSG_DBG,"EVDO power on!\n");

	sleep(5);

	m_iPhonePort = open("/dev/usb/tts/2", , O_RDWR);

	tcgetattr(m_iPhonePort, &options);
	
	options.c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|IXANY); 
	options.c_lflag &= ~(ECHO|ECHONL|ISIG|IEXTEN|ICANON);
	options.c_oflag &= ~OPOST;
	
	tcsetattr(m_iPhonePort,TCSANOW,&options);

	
}