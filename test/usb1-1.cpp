#include "../GlobalShare/ComuServExport.h"

int main()
{
	//IOSet(IOS_PHONEPOW, IO_PHONEPOW_ON, NULL, 0);
	system("insmod /dvs/extdrv/linux_usbserial.ko vendor=0x0547 product=0x2720");

	return 1;
}

