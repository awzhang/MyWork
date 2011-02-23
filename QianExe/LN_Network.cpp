#include "yx_QianStdAfx.h"

#if USE_LIAONING_SANQI == 1

#undef MSG_HEAD
#define MSG_HEAD	("QianExe-LN_Network ")

CLN_Network::CLN_Network()
{

}

CLN_Network::~CLN_Network()
{

}

int CLN_Network::Init()
{

}

int CLN_Network::Release()
{

}

int CLN_Network::DealSocketData(char *v_szBuf, DWORD v_dwLen)
{
	DWORD dwPacketNum = 0;
	g_objTransport.m_objRecvDataMng.PushData(LV2, v_dwLen, v_szBuf, dwPacketNum);
}

int CLN_Network::SendSocketData(char *v_szBuf, DWORD v_dwLen)
{
	char  szBuf[UDP_SENDSIZE] = {0};
	DWORD dwLen = 0;

	szBuf[0] = 0x04;	// 0x04表示QianExe UDP数据
	memcpy(szBuf+1, v_szBuf, v_dwLen);
	dwLen = v_dwLen+1;

	int iret = DataPush(szBuf, dwLen, DEV_QIAN, DEV_SOCK, LV2);

	if( 0 != iret )
	{
		PRTMSG(MSG_DBG, "DataPush one frame failed, iret=%d!\n",iret);
	}
}

#endif

