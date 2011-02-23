#ifndef _METERMOD_KAITIAN_H_
#define _METERMOD_KAITIAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UPLOAD_CNT	  5			//一次上传几比运营数据
#define TAXIDATASIZE   42        //一笔运营数据的大小
#define TAXIFILEPACKSIZE  64    //下载文件的每包的大小上限

#define TOTALDATACNT 200		//运营数据和刷卡数据存储容量  刷卡数据:200*5*42  运营数据:200*19 


#define ALABUF				1500		//收发缓冲区 
#define SERIALNUMBER  0x7006	//车台编号
 
#define  WAITPERIOD   120000  // 等待周期
#define  DOWNPERIOD   60000   // 下载业务定时器--zzg debug

#define  MAX_RESEND   5
  
#define TAXRECV       0x00
#define TAXOK         0x01 

#define TAXIFILEPATH				(L"\\ResFlh2\\TaxiFile")//计价器文件
#define TAXIDATAPATH				(L"\\ResFlh2\\TaxiData")//一卡通刷卡数据
#define METERDATAPATH				(L"\\ResFlh2\\MeterData")//单笔运营数据
//运营数据采集状态
#define METERDATA_GPS				0x0001	//位置信息
#define METERDATA_DATA			0x0002	//数据信息
#define METERDATA_SERVALUE	0x0004	//服务评价
#define METERDATA_COMPLETE	0x0006 //数据采集完成

extern Queue g_MeterRD;	     // 接收堆栈
extern Queue g_MeterSND;	 //发送堆栈
//extern Queue stack;        //计价器与车台


class CKTMeterMod  
{
private:
typedef struct TaxiFile
{
  INT8U 	 ChkSum;
  INT8U    id;                      // 下载文件id
  bool     flag;                    // 下载标志位
  bool     renewflag;               // 补采状态位
  INT16U   no;                     // 接收文件总包数
  INT8U    endlen;                  // 最后一包大小
	TaxiFile()
	{
		memset(this, 0, sizeof(*this));
		flag=false;
		renewflag=false;
	}
}TAXFILE_STRUCT;

#pragma pack(push,1)

struct DataHead
{
	int curIndex;  //当前待发送数据的位置
	int sendCnt;	 //待发送数据的个数
	DataHead(){memset(this, 0, sizeof(*this));}
};

struct TaxiFileHead
{
	byte fileID;	//文件ID
	WORD totalCnt;	//总包数
	WORD curNum;	//当前包序号
	byte endLen;    //最后一包长度
	TaxiFileHead(){memset(this, 0, sizeof(*this));}
};

struct frm2025
{
	byte resType;
	byte id;
	byte tolNum[2];
	byte curNum[2];
};

struct Up2027 
{
	byte id[2];		// 厂商ID
	byte cnt;		// 运营数据个数
	byte type;		// 运营数据类型
	byte len;		// 运营数据长度
	byte data[UPLOAD_CNT*TAXIDATASIZE];	// 运营数据
	
	Up2027()
	{
		memset(this, 0, sizeof(*this));
		id[0] = 0x27;
		id[1] = 0x27;
		//cnt = UPLOAD_CNT;
		len = 0;
	}
};

struct MeterData
{
	byte driverInfo[3];//司机信息
	byte year;//日期
	byte month;
	byte day;
	byte inTm[2];//上车时间（时分）
	byte outTm[2];//下车时间（时分）
	byte heavyTm;//重车时长
	byte meter[2];//计程
	byte time[2];//计时（时分）
	byte money[2];//金额
	byte emptyMeter[2];//空驶里程
	MeterData() {memset(this, 0, sizeof(*this));}
};

struct GPSInf
{
	byte date[3];//日期
	byte beginTime[2];
	byte beginLat[4];
	byte beginLon[4];
	byte endTime[2];
	byte endLat[4];
	byte endLon[4];
	GPSInf(){memset(this, 0, sizeof(*this));}
};

struct Up2068
{
	GPSInf gpsInf;//GPS信息
	byte meterDataLen;//运营数据长度
	MeterData meterData;//运营数据
	byte cnt;//载客人数
	byte serValue;//服务评价
	Up2068(){Init();}
	void Init()
	{
		memset(this,0,sizeof(*this));
		meterDataLen=sizeof(MeterData)+2;//zzg mod 090928
		cnt=1;
	}
};

struct frm4048
{
	byte data[5];//日期时间
	byte oldDriver[3];//原司机信息
	byte curDriverInfo[3];//现在司机信息
	frm4048(){memset(this, 0, sizeof(*this));}
};

#pragma pack(pop)

public:
	CKTMeterMod();
	virtual ~CKTMeterMod();
	
	int Init();
	int Release();
	
	void P_ThreadRecv();
	void P_ThreadWork();
	
	void P_PlayTxt(char *fmt);
	int DealComu48( char* v_szData, DWORD v_dwDataLen );
	int DealComu4A( char* v_szData, DWORD v_dwDataLen );
	
	void BeginMeterRcd();
	void SetSerValue(int v_iValue);
	void Test();
	
	void P_TmDownProc();
	void P_TmSetDeviceProc();

private:
	int m_iComPort;		// 串口句柄
	pthread_t m_pthreadRecv;
	pthread_t m_pthreadWork;
	bool m_bEscRecv;
	bool m_bEscWork;
	
	pthread_mutex_t m_mutexTaxiDataSta;//刷卡数据文件读写互斥
	pthread_mutex_t m_mutexMeterDataSta;//运营数据文件读写互斥
	TAXFILE_STRUCT   taxfile;

	Queue que_taxi;		// 刷卡数据临时存放队列
	Queue que_meter;	// 运营数据临时存放队列
	bool must_up;		// 数据上传标志
	bool last_pack;		// 计价器最后一笔刷卡数据
	bool link;//计价器是否连接正常

	Up2068 m_objUp2068;

	bool m_bJijiaEnable;//计价器是否启动
	byte m_DriverInf[3];
	byte m_DownCt;
  byte m_SendCt;
  byte m_DataType;
	byte m_SendSta;
	WORD m_DataLen;
	WORD m_wMeterSta;//运营数据状态
	WORD m_CurNo;

	typedef union 
	{
		WORD  word;
		struct 
		{
			byte  low;
			byte  high;
		} bytes;
	} WORD_UNION;

public:
	Queue tcprcv;	// TCP接收堆栈

private:
	int  _DealComChar(byte * chs,int len);
	void _DealCom(byte* frm, int len);
	bool _ComOpen();
	bool _ComClose();
	bool _ComSend(byte cmd, byte* data=NULL, int len=0);

	void _Work();
	void _TcpSend(byte trantype, byte datatype, byte *data, int len,int cvtType=CVT_NONE);
	bool _ChkAck(byte datatype);
	
	byte hdl_cent(byte type, byte *data, int dataLen);

  void hdl_recv0x00();
	void hdl_recv0x01();
	void hdl_recv0x02();
	void hdl_recv0x03(byte *data);
	void hdl_recv0x04(byte *data, WORD len);
	void hdl_recv0x05();
	void hdl_recv0x06(byte *data);
	void hdl_recv0x07();
	void hdl_recv0x08(byte *data);
	void hdl_recv0x09(byte *data);
	void hdl_recv0x0a(byte *data, WORD Len);
	void hdl_recv0x0b(byte *data, WORD Len);
	void hdl_recv0x0c(byte *data);
	void hdl_recv0x0d(byte *data);
  void hdl_recv2065(byte *Data, WORD Len);
	void hdl_recv2066(byte *Data);
	void hdl_recv2005();//禁用计价器
	void hdl_recv2006();//启用计价器

	bool _InitTaxiData();
	bool _InitMeterData();
	void _CheckTaxiFile();

	bool _FlushDataHead(byte type,WORD Cnt);
	bool _FlushTaxiDataRcd(byte *Data,WORD Len);
	bool _FlushMeterDataRcd(byte *Data,WORD Len);

	void _SendMeterData();//上传运营数据
	void _SendCashData();//上传刷卡数据

	bool _StorageFileData(byte *Data, WORD Len);
	bool _GetTaxiDataRcd();
	bool _GetMeterDataRcd();
};

#endif 
