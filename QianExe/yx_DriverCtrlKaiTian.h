#ifndef _YX_DRVCTRL_KAITIAN_H_
#define _YX_DRVCTRL_KAITIAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DRIVERPICPATH	L"\\Resflh2\\DriverPIC.jpg"
#define RCVPICPACKSIZE		1024//照片接收每包大小
#define SNDPICPACKSIZE		512//照片传输每包大小

class CKTDriverCtrl  
{
public:
#pragma pack(push,1)
	struct frm4004
	{
		byte driverID[3];//司机身份信息
		char driverName[10];//司机姓名
		byte level;//服务等级
		char copName[20];//公司名称	
		frm4004(){memset(this,0,sizeof(*this));}
	};
	
	struct frm4005
	{
		byte driverID[3];//司机身份信息
		WORD totalPack;//总包数
		byte winSize;//窗口大小
		frm4005(){memset(this,0,sizeof(*this));}
	};
	
	struct frm4006
	{
		byte driverID[3];//司机身份信息
		byte flag;//标志位
		WORD packNum;//包号
		WORD packSize;//包大小
		byte data[1024];//包内容
		frm4006(){memset(this,0,sizeof(*this));}
	};
	
	struct frm4045
	{
		byte driverID[3];//司机身份信息
		byte resType;
		frm4045(){memset(this,0,sizeof(*this));}
	};
	
	struct frm4046
	{
		byte driverID[3];//司机身份信息
		byte curWin;//当前窗口序号
		byte winSta[2];//窗口状态
		frm4046(){memset(this,0,sizeof(*this));}
	};
	
	//传输照片请求
	struct frmD6
	{
		byte type;
		byte driverID[3];//司机身份信息
		WORD packNum;//总包数	
		frmD6(){ memset(this,0,sizeof(*this));type=0xD6;}
	};
	
	//照片数据
	struct frmD8
	{
		byte type;
		WORD curPack;
		WORD dataLen;
		byte data[SNDPICPACKSIZE];
		frmD8(){ memset(this,0,sizeof(*this));type=0xD8;}
	};
	
	//交接班请求
	struct frmDC
	{
		byte type;
		byte driverID[3];//当前司机身份信息
		frmDC(){ memset(this,0,sizeof(*this));type=0xDC;}
	};
	
	//电子服务证信息
	struct frmD4
	{
		byte type;
		frm4004 info;
		frmD4(){ memset(this,0,sizeof(*this));type=0xD4;}
	};
#pragma pack(pop)

	struct PICRCVINFO
	{
		byte driverID[3];//司机身份信息
		WORD totalPack;//总包数
		WORD curPack;//当前包
		WORD curWin;//当前窗口序号
		byte winSize;//窗口大小
		byte winNum;//窗口总数
		byte lastWinSize;//最后一包窗口大小
		byte winSta[2];//窗口状态
		byte rcvSta;//接收状态0表示未接收完毕，1表示接收完毕
		PICRCVINFO(){memset(this,0,sizeof(*this));}
	};
	
	struct PICSNDINFO
	{
		byte driverID[3];//司机身份信息
		WORD packNum;//总包数
		WORD curPack;//当前包
		byte sendSta;//发送状态1表示未发送完，2表示已发送完
		PICSNDINFO(){memset(this,0,sizeof(*this));}
	};

  bool m_bSendPicWork;
  bool m_bSendPicExit;
  pthread_t	m_pthreadSendPic;//发送照片线程

private:
	PICRCVINFO m_objPicRcvInfo;
	PICSNDINFO m_objPicSndInfo;
	//byte m_bytSendPackNum;//当前发送包序号
	int m_iResPicTime;
	
public:
	CKTDriverCtrl();
	virtual ~CKTDriverCtrl();
  void Init();
  void Release();
  
  void P_TmReSendPicProc();
	void P_TmResPicProc();
  void P_ThreadSendPic();
  
	int DealComuD5( char* v_szData, DWORD v_dwDataLen );
	int DealComuD7( char* v_szData, DWORD v_dwDataLen );
	int DealComuD9( char* v_szData, DWORD v_dwDataLen );
	int DealComuDA( char* v_szData, DWORD v_dwDataLen );
	int DealComuDB( char* v_szData, DWORD v_dwDataLen );
	int Deal4004( char* v_szData, DWORD v_dwDataLen );
	int Deal4005( char* v_szData, DWORD v_dwDataLen );
	int Deal4006( char* v_szData, DWORD v_dwDataLen );
	int Deal4008( char* v_szData, DWORD v_dwDataLen );
	
	void ClearSendThd();
	bool SendPic();
	void BeginSendPic();
	void SndDrvChgInf(byte* v_aryODriverID,byte v_bytLen);
	
	void PicTest();//照片传输测试
	void DriverInfoTest();//请求电子服务证信息测试
	void DrvInfoChgTest();//交接班测试

private:
	WORD _GetPicFileSize();
	void _ReqDriverInfo(char* v_szDriverID,DWORD v_dwDataLen);
	bool _ReqSendPic();//请求发送
	void _Send4046();
};

#endif
