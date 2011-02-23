#ifndef _COMVIR_H_
#define _COMVIR_H_

//---------------------------------------------------------
//此类只对手机模块串口和复用通道的主设备进行操作，从设备又at和ppp各自操作
//---------------------------------------------------------

#define 	MAX_STR	1024

// 支持的模块类型
#define BENQM23				0		// ComVir_m23a_1152.dll
#define SIMCOM				1		// ComVir_sim_576.dll
#define BENQM25             2
#define BENQM33				3
#define M10                 4

#define BANDRATE			5		// 1:9600  2:19200  3:38400  4:57600  5:115200
// benq support: 2,3,4,5
// simcom support: all


#define IO_ZONE				0
#define DEBUGMODE 			0
#define BASIC_MODE 			0
#define ERR_MODE			0

// bits: Poll/final, Command/Response, Extension
#define PF 16
#define CR 2
#define EA 1

// the types of the frames
#define SABM 47
#define UA 99
#define DM 15
#define DISC 67
#define UIH 239
#define UI 3

// the types of the control channel commands
#define C_CLD 193
#define C_TEST 33
#define C_MSC 225
#define C_NSC 17

// V.24 signals: flow control, ready to communicate, ring indicator, data valid
// three last ones are not supported by Siemens TC_3x
#define S_FC 2
#define S_RTC 4
#define S_RTR 8
#define S_IC 64
#define S_DV 128

#define COMMAND_IS(command, type) ((type & ~CR) == command)
#define PF_ISSET(frame) ((frame->control & PF) == PF)
#define FRAME_IS(type, frame) ((frame->control & ~PF) == type)

//分配从手机模块串口接收的数据到各个通道
//channel：0，控制通道
#define MUXCHANNEL_AT	1	//channel：1，AT通道
#define MUXCHANNEL_PPP	2	//channel：2，拨号通道
#define MUXCHANNEL_MNG	3	//channel：3，管理通道


typedef struct GSM0710_Frame 
{
	unsigned char channel;
	unsigned char control;
	int data_length;
	unsigned char *data;
} GSM0710_Frame;

// Channel status tells if the DLC is open and what were the last
// v.24 signals sent
typedef struct Channel_Status 
{
	int opened;
	unsigned char v24_signals;
} Channel_Status;

class CComVir
{
public:
	CComVir();
	~CComVir();
	int Init();
	int Release();
	bool InitComVir();
	void ReleaseComVir();
	int P_RecvPhoneDataThread();
	int P_RecvAtMuxDataThread();
	int P_RecvPppMuxDataThread();
	bool GetComMuxPath(DWORD v_pathSymb, char* v_pPath, DWORD v_PathLen);
	
private:

	char* ptsname(int fdMst);
	int grantpt(int fdm);
	int unlockpt(int fdm);
	int _ComMuxOpen(char* v_pSlavePath);
	void _StopRecvThread();
	int _WriteToSerial(void* szSendBuf, unsigned long dwSendLen);
	bool _SendCmd(char* cmd);
	void _QueryModuleVer();
	bool _InitMux();
	void _CloseMux();
	bool _ComMuxClose();
	bool _ComPhyOpen();
	bool _ComAllClose();
	int _AssignDataToChannels(unsigned char * data, unsigned long data_length, unsigned char channel);
	unsigned char _MakeFcs(const unsigned char *input, int count);
	int _WriteFrame(int channel, const char *input, int count, unsigned char type);
	void _HandleCommand(GSM0710_Frame * frame);
	void _DealChar(unsigned char ch);
	void _DealFrame(GSM0710_Frame *frame);
	void _GsmPowOn();
	void _GsmHardReset();
	void _GsmPowOff();
	void StartComVir();
	void _InitFlagReset();
	
	bool _InitStatic();

	
public:
	
	
private:

	int m_FdMstAt;	//at通道主设备伪终端句柄
	int m_FdMstPpp;	//ppp通道主设备
//	int m_FdMstMng;	//管理通道主设备

	int m_FdSlvAt;	//at通道从设备伪终端句柄
	int m_FdSlvPpp;	//ppp通道从设备
	int m_FdSlvMng;	//ppp通道从设备

	char m_szSlvPathAt[32];	//at通道从设备伪终端句柄
	char m_szSlvPathPpp[32];	//ppp通道从设备
	char m_szSlvPathMng[32];	//管理通道从设备

	int m_FdComPhyPort;		//手机模块硬件串口

	bool m_bComInMux;	//mux初始化标志
	bool m_bKillRecvThread;	//关闭串口接收线程标志
	int m_gsmtype;		//模块类型
	bool m_bModuleVerHigh;	// 该值目前仅对Benq M23A有意义

	//接收缓冲区
	char m_CommBuffer[MAX_STR];
	int m_CommBufLen;

	bool m_ChannelSend[4];
	Channel_Status m_cstatus[4];

	//互斥量
	pthread_mutex_t 	m_MutexWrite;

};

#endif	//#define _COMVIR_H_

