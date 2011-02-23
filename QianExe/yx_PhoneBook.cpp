#include "yx_QianStdAfx.h"


#if USE_TELLIMIT == 1
//------------------------------------------------------------------------------
// 处理中心协议
void CPhoneBook::hdl_cent(byte datype, char *dat, ulong len)
{
	for(ulong i=0; i<len; i++) 
	{
		if(0x7f==*(dat+i))
			*(dat+i) = 0;
	}

	switch(datype)
	{
	case 0x01:
		hdl_3301((byte*)dat, len);
		break;
		
	case 0x02:
		hdl_3302((byte*)dat, len);
		break;
		
	default:
		break;
	}
}

//------------------------------------------------------------------------------
// 往中心发送TCP数据
void CPhoneBook::tcp_send(byte trantype, byte datatype, void *data, int len)
{
	int ln = 0;
	byte buf[2048] = {0};

	// 将0转化为0x7f 
	memcpy(buf, data, len);
	for(int i=0; i<len; i++)
	{
		if(0 == buf[i])	
			buf[i] = 0x7f;
	}
	ln = len;
	
	int ln2 = 0;
	char buf2[2048] = {0};
	int iRet = g_objSms.MakeSmsFrame((char*)buf, ln, trantype, datatype, buf2, sizeof(buf2), ln2);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( buf2, ln2, LV9 );
}


void CPhoneBook::hdl_3301(byte *dat, ulong len)
{
	frm3301 *p = (frm3301*)dat;
	bool succ = true;
	
	for(int i=0; i<p->cnt; i++)
	{
		byte idx = p->book[i].idx;

		if(idx>30 || 0==idx)
		{
			succ = false; 
			break; 
		}

		memcpy(&m_cfg.book[idx-1], &p->book[i], sizeof(Book));
	}

// 见鬼，千里眼从1开始编号，而king从0开始编号
// { 临时为了黑龙江办连king
// 	for(int i=0; i<p->cnt; i++) {
// 		byte idx = p->book[i].idx;
// 		if( 0x7f == idx ) idx = 0; // 见鬼king怎么改为7f了
// 		if(idx>=30 )	{ succ = false; break; }
// 		memcpy(&m_cfg.book[idx], &p->book[i], sizeof(Book));
// 	}
// } 临时为了黑龙江办连king
	
	// 保存配置
	if(succ) 
	{
		int ret = SetSecCfg(&m_cfg, offsetof(tagSecondCfg, m_uniBookCfg.m_szReservered), sizeof(m_cfg) );
		if(0 != ret)
			succ = false;
	}
	
	// 往中心发送应答
	frm3341 frm;
	frm.reply = (succ) ? 0x01 : 0x7f;
	frm.frmno = p->frmno;
	tcp_send(0x33, 0x41, &frm, sizeof(frm));
	
	PRTMSG(MSG_NOR, "hdl_3301 Set Book(%u)%s!\n", p->frmno, succ ? "succ" : "fail");
}


void CPhoneBook::hdl_3302(byte *dat, ulong len)
{
	frm3302 *p = (frm3302*)dat;
	
	if(0==p->cnt) 
	{
		memset(&m_cfg.tel[0][0], 0, sizeof(m_cfg.tel));
	}

	for(int i=0; i<p->cnt; i++) 
	{
		memcpy(&m_cfg.tel[i][0], &p->tel[i][0], 15);
	}
	
	int ret = SetSecCfg(&m_cfg, offsetof(tagSecondCfg, m_uniBookCfg.m_szReservered), sizeof(m_cfg) );
	
	// 往中心发送应答
	frm3342 frm;
	frm.reply = (0==ret) ? 0x01 : 0x7f;
	tcp_send(0x33, 0x42, &frm, sizeof(frm));
	
	PRTMSG(MSG_NOR, "hdl_3302 Set serve tel %s!\n", (0==ret) ? "succ" : "fail");
}	

#endif

