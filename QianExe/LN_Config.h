#ifndef _LN_CONFIG_H_
#define _LN_CONFIG_H_



class CLN_Config
{
public:
	CLN_Config();
	virtual ~CLN_Config();

public:
	int Deal01_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal01_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal01_0004(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal01_0006(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal01_0007(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int Deal09_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal09_0008(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal09_0009(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal09_0010(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal09_0011(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal09_0012(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int Deal0D_0000(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal0E_0000(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int Deal11_0001(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0002(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0003(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0005(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0006(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0007(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0008(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0009(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_000E(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_000F(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0014(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0016(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0018(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0019(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int Deal11_0022(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0023(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0024(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0025(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0026(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0027(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0028(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal11_0029(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int Deal12(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);
	int Deal15(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ);

	int DealAreaRpt(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, char AreaAttr, ushort Action);
	int DealAreaPark(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, char AreaAttr, ushort Action);
	int DealRegionSpd(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, char AreaAttr, ushort Action);
	int DealLineRpt(char *v_szBuf, DWORD v_dwLen, DWORD v_dwCenterSEQ, ushort Action);
	bool RemoveRpt(ushort Action, char *Rid, char *sz_Buf, DWORD *v_dwLen);
	void FlushCntAndInterval(ushort v_usTimes, ushort v_usInterval);
};


STRUCT_1 tagAKV_Time
{
	byte	m_bytAttr;
	byte	m_bytKlen;
	char	m_szKey;
	byte	m_bytVlen[2];
	char	m_szValue[6];

	tagAKV_Time()
	{
		memset(this, 0, sizeof(*this));

		m_bytAttr = 0;
		m_bytKlen = 1;
		m_szKey = 't';
		m_bytVlen[0] = 0;
		m_bytVlen[1] = 6;
	}
};

STRUCT_1 tagAKV_Error
{
	byte	m_bytAttr;
	byte	m_bytKlen;
	char	m_szKey;
	byte	m_bytVlen[2];
	char	m_szValue[4];
	
	tagAKV_Error()
	{
		memset(this, 0, sizeof(*this));
		
		m_bytAttr = 0;
		m_bytKlen = 1;
		m_szKey = 'm';
		m_bytVlen[0] = 0;
		m_bytVlen[1] = 4;
		memcpy(m_szValue, "null", 4);
	}
};

STRUCT_1 tagAKV_s
{
	byte	m_bytAttr;
	byte	m_bytKlen;
	char	m_szKey;
	byte	m_bytVlen[2];
	char	m_szValue;
	
	tagAKV_s()
	{
		memset(this, 0, sizeof(*this));
		
		m_bytAttr = 0;
		m_bytKlen = 1;
		m_szKey = 's';
		m_bytVlen[0] = 0;
		m_bytVlen[1] = 1;
		m_szValue = 0x01;
	}
};

STRUCT_1 tagReply01_0001
{
	byte	m_bytAck;
	tagAKV_Time  m_objAKVTime;
	tagAKV_Error m_objAKVError;

	tagReply01_0001()
	{
		m_bytAck = 1;
	}
};

STRUCT_1 tagReply01_0002
{
	byte	m_bytAck;
	tagAKV_Error m_objAKVError;

	tagReply01_0002()
	{
		m_bytAck = 1;
	}
};

STRUCT_1 tagReply01_0004	// tagReply01_0006结构与此相同，不再重复定义
{
	byte	m_bytAck;
	tagAKV_s	 m_objAKVs;
	tagAKV_Error m_objAKVError;

	tagReply01_0004()
	{
		m_bytAck = 1;
	}
};


#endif


