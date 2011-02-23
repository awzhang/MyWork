#include "yx_QianStdAfx.h"

#undef MSG_HEAD 
#define MSG_HEAD	("QianExe-DevCtrlKaiTian")

CKTDeviceCtrl::CKTDeviceCtrl()
{
	m_bytDeviceSta = 0;
}

CKTDeviceCtrl::~CKTDeviceCtrl()
{

}

//ø≠ÃÏ…Ë±∏ºÏ≤‚
int CKTDeviceCtrl::DealComuAD( char* v_szData, DWORD v_dwDataLen )
{
	frmE9 frm;
	memcpy(&frm,v_szData,sizeof(frm));
	//SetDeviceSta(KT_LED,true);
	//SetDeviceSta(KT_METER,true);
	frm.touchSta = 0x01;
	if(m_bytDeviceSta & KT_LED) 
		frm.ledSta=0x01;
	if(m_bytDeviceSta & KT_METER) 
		frm.meterSta=0x01;
	if(m_bytDeviceSta & KT_PHOTO) 
	  frm.photoSta=0x01;
//	if(g_objPhoto.GetPhotoSta()) 
//		frm.photoSta=0x01;
	
	char buf[256];
	buf[0]=0x01;
	memcpy( buf+1, (char*)&frm, sizeof(frm) );
	DataPush(buf, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
	
	PRTMSG(MSG_NOR, "Rcv ComuAD Sta:%x\n",m_bytDeviceSta);
	return 0;
}

void CKTDeviceCtrl::SetDeviceSta( DEV_STATE v_deviceSta,bool v_bValid )
{
	if(v_bValid) 
		m_bytDeviceSta |= v_deviceSta;
	else 
		m_bytDeviceSta &= ~v_deviceSta;
}

byte CKTDeviceCtrl::GetDeviceSta()
{
	return m_bytDeviceSta;
}
