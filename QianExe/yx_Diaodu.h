#ifndef _YX_DIAODU_H_
#define _YX_DIAODU_H_

class CDiaodu
{
public:
	CDiaodu();
	virtual ~CDiaodu();

	int Deal0501( char* v_szData, DWORD v_dwDataLen );
	int Deal053c( char* v_szData, DWORD v_dwDataLen );
	int Deal053d( char* v_szData, DWORD v_dwDataLen );

	//zzg add
	int Deal0510( char* v_szData, DWORD v_dwDataLen );
	int Deal0511( char* v_szData, DWORD v_dwDataLen );
	int DealComu73( char* v_szData, DWORD v_dwDataLen );
	int DealComu7B( char* v_szData, DWORD v_dwDataLen );
	int DealComu3C( char* v_szData, DWORD v_dwDataLen );
	
	void Init();
	
	int Deal4208( char* v_szData, DWORD v_dwDataLen );
	
	void Send0550();
	void DzCheck(bool v_bResult);
	
	void DzTest();
	void LedTest();
private:
	struct frm0510
	{
		byte type;//电召类型
		byte preTime[2];//预约时间
		char fromAdd[20];//召车人地点
		char endAdd[20];//到达地点	
		frm0510(){memset(this,0,sizeof(*this));}
	};
	struct frm0511
	{
		byte type;//电召类型
		byte preTime[2];//预约时间
		byte earlyTime;//提前时间
		char tel[15];//联系人电话
		char name[10];//联系人姓名
		char fromAdd[20];//召车人地点
		char endAdd[20];//到达地点
		frm0511(){memset(this,0,sizeof(*this));}
		
	};
	//车辆指示应答
#pragma pack(push,1)
	struct Frm12 {
		byte type;
		byte len;
		char num[256];
		Frm12() { memset(this, 0, sizeof(*this));  type = 0x12; }
	};
	struct frm0541
	{
		BYTE	m_bytLen; //数据长度
		BYTE	m_bytCheck; //校验和
		BYTE	m_szRouter[4];//路由信息
		BYTE	m_bytDataType;//数据类型 02为车辆指示
		BYTE	m_bytEvtType;//事件类型
		BYTE	m_bytDataLen;//数据长度
		char	m_szData[200];//数据内容
		frm0541(){memset(this,0,sizeof(*this));}
		
	};
#pragma pack(pop)
	
	frm0510 m_objDzInfo;//预约信息
	frm0541 m_objDiaoduInfo;//用于存放车辆指示信息
	
	public:
		void P_TmSetBespokeProc();
		void P_TmBespokeNotProc();
		friend void G_TmSetBespokeProc();
		friend void G_TmBespokeNotProc();
};

#endif


