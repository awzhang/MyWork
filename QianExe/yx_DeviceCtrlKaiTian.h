#ifndef _YX_DEVCTRL_KAITIAN_H_
#define _YX_DEVCTRL_KAITIAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKTDeviceCtrl  
{
public:
	CKTDeviceCtrl();
	virtual ~CKTDeviceCtrl();

	int DealComuAD( char* v_szData, DWORD v_dwDataLen );
	void SetDeviceSta( DEV_STATE v_deviceSta,bool v_bValid );
	byte GetDeviceSta();

private:
#pragma pack(push,1)
	struct frmE9
	{
		byte type;
		byte sta;
		byte gpsSta;
		byte gprsSta;
		byte meterSta;
		byte ledSta;
		byte touchSta;
		byte photoSta;
		frmE9()
		{
			memset(this,0x02,sizeof(*this));
			type=0xe9;
			sta=0x00;
			touchSta=0x01;
		}
	};
#pragma pack(pop)

	byte m_bytDeviceSta;//从低位到高位分别表示：计价器0x0001、LED0x0002、触摸屏0x0004、摄像头状态0x0008，0为异常，1为正常
};

#endif 
