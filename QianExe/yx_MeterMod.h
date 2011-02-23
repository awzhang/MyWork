#ifndef _METERMOD_H_
#define _METERMOD_H_

class CMeterMod
{
public:

	CMeterMod();
	virtual ~CMeterMod();

private:

#pragma pack(push,1)

	struct PointInfo
	{
		byte date[3];	// 日期
		byte s_tm[2];	// 起点时间
		byte s_lat[4];	// 起点纬度
		byte s_lon[4];	// 起点经度
		byte e_tm[2];	// 终点时间
		byte e_lat[4];	// 终点纬度
		byte e_lon[4];	// 终点经度
	
		PointInfo() 	{ memset(this, 0, sizeof(*this)); }
	};
	
	
	struct Up2022 
	{
		byte num;		// 编号I
		byte id[2];		// 厂商ID
		byte cnt;		// 运营数据个数
		byte ptif[23];	// 起点终点位置信息
		byte len;		// 运营数据长度
		byte data[60];	// 运营数据
		
		Up2022()
		{
			memset(this, 0, sizeof(*this));
			num = 1;
			id[0] = 0x01;
			id[1] = 0x02;
			cnt = 1;
			len = 60;
		}
	};
	
	
	
#pragma pack(pop)

private:
	pthread_t m_hInvalidCarry;
	pthread_t m_hthJijiaRead;
	pthread_t m_hthJijiaWork;
	int m_hJijia;		// 串口句柄

	PointInfo ptif;		// 起点终点定位信息
	Queue que_up;		// 运营数据临时存放队列
	bool must_up;		// 运营数据上传标志
	volatile bool m_bCarrySkip;//载客不打表状态重记

public:
	Queue tcprcv;	// TCP接收堆栈	
	
public:
	int  Init();
	int  Release();
	
	bool JijiaComOpen();
	
	DWORD JijiaRead();
	DWORD JijiaWork();
	DWORD InvalidCarry();
	int Deal1015( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	int Deal0119( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen, bool v_bFromSM );
	void DealComBuf(char *v_szBuf, int v_iLen);
	
private:
	void _DealComChar(byte ch);
	void _DealCom(byte* frm, int len);
	bool _ComSend(byte cmd, byte* data=NULL, int len=0);
	void _TcpSend(byte trantype, byte datatype, byte *data, int len);
	void _Work();
	bool _ChkAck(byte datatype);
	byte _DealCentData(byte type, byte *data, int len);
	void _SetFull(PointInfo *p);
	void _SetEmpty(PointInfo *p);

	bool _CheckSta();
	void _InvCarryRpt(const tagGPSData &v_objGps);
	void _Snd0159( tagQianGps &v_objQianGps );

};


#endif	//#define _METERMOD_H_

