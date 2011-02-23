#include "yx_QianStdAfx.h"

#if USE_DRVREC == 1

void *G_ThreadDrvRecWork(void *arg)
{
	g_objDriveRecord.P_ThreadDrvRecWork();
}

void G_TmDrvRecTimer(void *arg, int len)
{
	g_objDriveRecord.P_TmDrvRecTimer();
}

#undef MSG_HEAD
#define MSG_HEAD ("QianExe-DriveRecord")

//////////////////////////////////////////////////////////////////////////

CDriveRecord::CDriveRecord()
{
	m_pthreadWork = 0;
	m_bytAreaCode = 0;

	m_bNeedClt = false;
	m_v.init();				//变量初始化
	m_bLcdLog = false;
	m_bDrvRecQuit = false;
#if USE_LIAONING_SANQI == 1
	TotalMeter = 0;
	Mile = 0;
#endif
}

CDriveRecord::~CDriveRecord()
{

}

int CDriveRecord::Init()
{
	// 初始化配置以及文件
	init_filecfg();

	// 创建工作线程
	if( pthread_create(&m_pthreadWork, NULL, G_ThreadDrvRecWork, NULL) != 0)
	{
		PRTMSG(MSG_ERR, "Create DrvRec Thread failed!\n");
		return ERR_THREAD;
	}

	// 注册定时器
	_SetTimer(&g_objTimerMng, DRIVERECORD_TIMER, 1000, G_TmDrvRecTimer);

	return 0;
}

int CDriveRecord::ReInit()
{
	HRcdFile objHRcdFile;
	objHRcdFile.init();

	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb");
	if( NULL != fp) 
	{
		static bool bFirstReInit = true;

		fread(&objHRcdFile, 1, sizeof(objHRcdFile), fp);

		// 将部分变量用最新的值来代替
		objHRcdFile.ticks = m_v.ticks;					//定时计数(秒)
		objHRcdFile.numdrv = m_v.numdrv;				//当前司机编号
		objHRcdFile.stacar = m_v.stacar;				//当前车辆的状态(只有两种状态,移动和静止)
		objHRcdFile.stacur = m_v.stacur;				//当前车辆状态(四种状态)
		objHRcdFile.stapre = m_v.stapre;				//上次车辆状态(四种状态)
		objHRcdFile.run = m_v.run;				//车辆连续行驶时间(分)
		objHRcdFile.stop = m_v.stop;			//车辆连续停止时间(分)
		objHRcdFile.inputflag = m_v.inputflag;			//司机登陆结果(调度屏返回)
		objHRcdFile.speed = m_v.speed;					//当前速度(公里/时)
		memcpy(objHRcdFile.vector, m_v.vector, 60);		//最近60秒的速度(海里/时)
		memcpy(objHRcdFile.average, m_v.average, 15);	//最近15分钟的平均速度
		objHRcdFile.lstlat = m_v.lstlat;				//最后一个有效的经度
		objHRcdFile.lstlon = m_v.lstlon;				//最后一个有效的纬度
		objHRcdFile.metercur = m_v.metercur;			//当次行驶里程数(m)
		
		if( true == bFirstReInit )
		{
			bFirstReInit = false;
			objHRcdFile.metertotal += m_v.metertotal;		//总里程(m)
		}

		memcpy(objHRcdFile.rp_doub, m_v.rp_oil, 5);		//标志收到 请求上传行驶数据的应答
		objHRcdFile.uptime = m_v.uptime;		//已主动上报的时长
		objHRcdFile.st_clt = m_v.st_clt;		//带油耗主动上报采集状态
		objHRcdFile.updoub = m_v.updoub;				//是否需要上报疑点数据的标志, 
		objHRcdFile.tickdoub = m_v.tickdoub;			//事故疑点停车时间计数
		memcpy(objHRcdFile.rp_doub, m_v.rp_doub, 2);	//标志收到 实时事故疑点上传应答
		objHRcdFile.st_log = m_v.st_log;				//司机登陆状态	
		objHRcdFile.prompt = m_v.prompt;				//非法启动已提示次数
		objHRcdFile.alarmed = m_v.alarmed;		//非法启动是否报警过的标志

		//然后替换掉现在的m_v
		memcpy((void*)&m_v, (void*)&objHRcdFile, sizeof(m_v));
		PRTMSG(MSG_DBG, "m_v renew!\n");
	}
	else
	{
		fp = fopen(DRIVE_RECORD_FILENAME, "wb+");
		if( NULL != fp)
		{	
			fwrite(&objHRcdFile, 1, sizeof(objHRcdFile), fp);
			PRTMSG(MSG_NOR,"Create DrvJg2 Succ!\n");
		}
		else
		{
			PRTMSG(MSG_NOR,"Create DrvJg2 Fail\n");
			perror("");
		}
	}
	
	if( NULL != fp )
		fclose(fp);
}

int CDriveRecord::Release()
{
	flush_headrcd();
}

// 初始化配置文件
void CDriveRecord::init_filecfg()
{
	// 读取车台配置到m_cfg中
	uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	GetSecCfg(&m_cfg, sizeof(m_cfg), offset, sizeof(m_cfg));
	
	if(123!=m_cfg.inited) // 注意，改为判断特殊的标志123
	{
		m_cfg.init(); 
		SetSecCfg(&m_cfg, offset, sizeof(m_cfg));
		PRTMSG(MSG_NOR, "Init DrvCfg!\n");
	}

	// 读取越界区域报警设置参数
	GetSecCfg((void*)&m_objAreaCfg, sizeof(m_objAreaCfg), offsetof(tagSecondCfg, m_uni2QAreaCfg.m_obj2QAreaCfg), sizeof(m_objAreaCfg));
	
// 	// 初始化数据文件
// 	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb");
// 	if(fp) 
// 	{
// 		fread(&m_v, 1, sizeof(m_v), fp);
// 
// 		if( Logined != m_v.st_log ) // 使得启动时就可以提示司机登陆
// 			m_v.st_log = UnLog; 
// 
// 		m_v.st_clt = 0; // 090817 xun add,使得启动时行驶记录上报状态机能从初始状态开始
// 	}
// 	else
// 	{
// 		fp = fopen(DRIVE_RECORD_FILENAME, "wb+");
// 		if(fp)
// 			fwrite(&m_v, 1, sizeof(m_v), fp);
// 
// 		PRTMSG(MSG_NOR,"Create DrvJg2 %s!\n", fp ? "Succ" : "Fail");
// 	}
// 
// 	fclose(fp);
}

void CDriveRecord::P_ThreadDrvRecWork()
{
#define TM_DOUB1	100		//非正式停车时间
#define TM_DOUB2	300		//正式停车时间

	bool cur;
	int i=0;

	// 200毫秒执行一次, 用于记录和保存事故疑点信息并上报事故疑点
	while( !g_bProgExit && !m_bDrvRecQuit)
	{
		usleep(200000);

		// --------- 保存事故疑点数据 -----------------------
		cur = (m_v.vector[0]>VT_LOW_RUN) && acc_valid();
		
		if(m_v.tickdoub>=TM_DOUB1 && m_v.tickdoub<TM_DOUB2 && cur)	//停车在20s-60s之间
		{	
			m_v.rcddoub.stoptype = 0x01;	//非正式疑点
			flush_doubrcd();	
		}

		if(TM_DOUB2==m_v.tickdoub)	//停车超过60s,将上报标志置1
		{	
			m_v.rcddoub.stoptype = 0x02;	//正式疑点
			flush_doubrcd(); 
			if(!m_v.updoub)		m_v.updoub = 1;		//避免中断上报的流程
		}		
		
		if(cur)
		{
			m_v.tickdoub = 0;
			static int cnt = 0;
			if(cnt++>=4)	//1秒保存一次GPS
			{			
				cnt = 0;
				m_v.rcddoub.set_data(true, get_sensor());
			} 
			else 
			{ 
				m_v.rcddoub.set_data(false, get_sensor());
			}
		} 
		else
		{
			m_v.tickdoub++;		//累加停车时间
		}

#if USE_PROTOCOL == 2
		continue;		// 公开版，山东九通项目，不主动上传事故疑点数据
#endif
		
		// --------- 上报事故疑点数据 ----------------------
		static enum {
			Idlesse, Sending, WaitReply, 
		} st_snd = Idlesse;
		static ulong index = 0;		//当前记录的在记录文件中的索引
		static int overtime = 0;	//发送超时计数
		static byte frmno = 0;		//帧序号
		
		switch(st_snd) 
		{
		case Idlesse:	// 空闲状态
			{
				index = 0;
				overtime = 0;

				if(m_v.updoub)	
					st_snd = Sending;
			}
			break;
			
		case Sending:	// 发送状态
			{
				RcdDoub rcd;
				index = get_lstdoub(m_v.updoub, rcd);
				if(0==index)
				{
					PRTMSG(MSG_NOR, "No find lstdoub!\n"); 
					m_v.updoub = 0; 
					st_snd = Idlesse;
					break;
				}
				
				if(++frmno>0x0f)
					frmno = 0;
				
				// 发送前半条记录
				frm0157 frm = {0};
				frm.trantype = (frmno<<4) | 0x01;
				memcpy(frm.e_time, rcd.times, 6);

				for(i=0; i<10; i++) 
				{
					memcpy(frm.doub[i].lon, rcd.pos[i].lon, 4);
					memcpy(frm.doub[i].lat, rcd.pos[i].lat, 4);
					frm.doub[i].speed = rcd.vector[i*5];
					frm.doub[i].sensor = rcd.sensor[i*5];
				}
				frm.doubtype = rcd.stoptype;
				tcp_send(0x01, 0x57, &frm, sizeof(frm));
				
				// 发送后半条记录
				frm.trantype = (frmno<<4) | 0x02;
				memcpy(frm.e_time, rcd.times, 6);
				for(i=0; i<10; i++)
				{
					memcpy(frm.doub[i].lon, rcd.pos[i+10].lon, 4);
					memcpy(frm.doub[i].lat, rcd.pos[i+10].lat, 4);
					frm.doub[i].speed = rcd.vector[i*5+50];
					frm.doub[i].sensor = rcd.sensor[i*5+50];
				}
				frm.doubtype = rcd.stoptype;
				tcp_send(0x01, 0x57, &frm, sizeof(frm));
				
				// 清除rp_doub
				m_v.rp_doub[0] = 0;
				m_v.rp_doub[1] = 0;
				
				st_snd = WaitReply;
				PRTMSG(MSG_NOR, "Send doub and wait rp_doub(%u)..\n", index);
			}
			break;
			
		case WaitReply:	// 等待中心应答
			{
				bool rcv1 = ( m_v.rp_doub[0] == ((frmno<<4)|0x01) );
				bool rcv2 = ( m_v.rp_doub[1] == ((frmno<<4)|0x02) );

				if( rcv1 && rcv2 )	//收到应答(2帧的应答都收到)
				{
					PRTMSG(MSG_NOR, "Rcv rp_doub!\n");
					set_doubtype(index-1);

					if(1==m_v.updoub)
						m_v.updoub = 0;

					st_snd = Idlesse; 
					break; 
				}
				
				if(++overtime > 3*300)	//中心应答超时!
				{
					PRTMSG(MSG_NOR, "Wait rp_doub overtime!\n");
					m_v.updoub = 0;
					st_snd = Idlesse; 
					break; 
				}
			}
			break;
			
		default:
			break;
		}
	}
}

void CDriveRecord::P_TmDrvRecTimer()
{
	tagGPSData gps(0);
	GetCurGps(&gps, sizeof(gps), GPS_LSTVALID);
	
	m_v.ticks++;
	
	// 设置车辆状态(移动或静止)
	set_car_state(gps);
	
	// 处理里程统计
	deal_meter(gps);
	
	// 检查是否超速并报警
	chk_overspeed();
	
	// 检查是否低速并报警
	chk_belowspeed();
	
	// 处理司机登陆
	deal_login();	
	
	// 处理中心协议
	deal_center();
	
	// 处理车辆行使记录
	deal_state();
	
	// 处理报警区域
	deal_alarm(gps);

	// 处于越界区域（0F 协议类型）
	deal_area(gps);
	
	// 处理查询数据的上传
	deal_query();

#if 0 == USE_PROTOCOL	
#if USE_LIAONING_SANQI == 0
	// 处理主动上报行驶数据(带油耗)
	deal_updrv(gps);
#endif
#endif
}

//------------------------------------------------------------------------------
// 处理中心的协议
void CDriveRecord::deal_center()
{
	// 处理中心协议	
	byte tmp[2048] = {0};
	ulong len = tcprcv.pop(tmp);
	if(len==0 || len>2048)	return;

	// 实时事故疑点协议
	if(0x01 == tmp[0])
	{
		for(ulong i=2; i<len; i++)	
		{ 
			if(0x7f==tmp[i])	tmp[i] = 0; 
		}
		switch(tmp[1]) 
		{
			case 0x17:	
				hdl_0117(tmp+2, len-2);		
				break;	//实时事故疑点上传应答		
			case 0x27:	
				hdl_0127(tmp+2, len-2);		
				break;	//非正式疑点上传请求			
			default:	
				PRTMSG(MSG_NOR, "Rcv unknown frm(%02x,%02x)!\n", tmp[0], tmp[1]);	
				break;
		}
	}

	// 里程统计业务协议
	if(0x09 == tmp[0]) 
	{
		for(ulong i=2; i<len; i++)	
		{ 
			if(0x7f==tmp[i])	
				tmp[i] = 0; 
		}
		switch(tmp[1]) 
		{
			case 0x01:	
				hdl_0901(tmp+2, len-2);		
				break;	//总里程查询请求		
			case 0x02:	
				hdl_0902(tmp+2, len-2);		
				break;	//总里程清除请求			
			case 0x04:	
				hdl_0904(tmp+2, len-2);		
				break;	//查询最近360小时内和2个日历天的行驶里程请求
			case 0x05:	
				hdl_0905(tmp+2, len-2);		
				break;	//里程记录查询
			default:	
				PRTMSG(MSG_NOR, "Rcv unknown frm(%02x,%02x)!\n", tmp[0], tmp[1]);	
				break;
		}
	}

	// 用户设置业务协议
	if(0x10 == tmp[0]) 
	{
		for(ulong i=2; i<len; i++)	
		{ 
			if(0x7f==tmp[i])	
				tmp[i] = 0; 
		}
		switch(tmp[1]) 
		{
			case 0x3e:	
				hdl_103e(tmp+2, len-2);		
				break;	//设置定时上传行驶数据		
			default:	
				PRTMSG(MSG_NOR, "Rcv unknown frm(%02x,%02x)!\n", tmp[0], tmp[1]);	
				break;
		}
	}

	// 行车记录仪业务协议
	if(0x36 == tmp[0]) 
	{
		byte tmp2[2048] = {0};
		for(int k=0; k<len; k++)
		{
			if( 0x7f == tmp[k] )	
				tmp[k] = 0;
		}
		int len2 = CodeXToCode8(tmp2, tmp+2, len-2, 7);	//将用户数据进行7->8的转换
		switch(tmp[1]) 
		{
			case 0x01:	
				hdl_3601(tmp2, len2);	
				break;	//设置车辆信息请求		
			case 0x02:	
				hdl_3602(tmp2, len2);	
				break;	//请求设置驾驶员身份			
			case 0x03:	
				hdl_3603(tmp2, len2);	
				break;	//设置疲劳行驶时间
			case 0x04:	
				hdl_3604(tmp2, len2);	
				break;	//查询事故疑点数据请求
			case 0x05:	
				hdl_3605(tmp2, len2);	
				break;	//查询最近15分钟内每分钟平均速度请求	
			case 0x06:	
				hdl_3606(tmp2, len2);	
				break;	//查询连续行驶时间记录请求
			case 0x11:	
				hdl_3611(tmp2, len2);	
				break;	//设置超速参数
			case 0x13:	
				hdl_3613(tmp2, len2);	
				break;	//采集司机信息
			case 0x18:	
				hdl_3618(tmp2, len2);	
				break;
			//case 0x19:
			//	hdl_3619(tmp2, len2);	
			//	break; //设置低速参数
			case 0x23:
				hdl_3623(tmp2, len2);
				break;
			default:	
				PRTMSG(MSG_NOR, "Rcv unknown frm(%02x,%02x)!\n", tmp[0], tmp[1]);	
				break;
		}
	}
	
	// 行车记录仪报警业务协议
	if(0x37 == tmp[0]) 
	{
		byte tmp2[2048] = {0};
		int len2 = CodeXToCode8(tmp2, tmp+2, len-2, 7);	// 将用户数据进行7->8的转换
		switch(tmp[1]) 
		{
#if USE_OIL == 3
			case 0x10:
				g_objOil.Deal3710(tmp2, len2);
				break;
				
			case 0x11:
				g_objOil.Deal3711(tmp2, len2);
				break;
				
			case 0x12:
				g_objOil.Deal3712(tmp2, len2);
				break;
				
			case 0x13:
				g_objOil.Deal3713(tmp2, len2);
				break;
#endif
			case 0x01:	
				hdl_3701(tmp2, len2);	
				break;	// 设置行驶时间段请求	
				
			case 0x02:	
				hdl_3702(tmp2, len2);	
				break;	// 设置报警区域请求

#if USE_OIL == 3
			case 0x09:	
				hdl_3709(tmp2, len2);	
				break;	// 请求上传行驶数据应答(带油耗值)
#else
			case 0x07:	
				hdl_3707(tmp2, len2);	
				break;	// 请求上传行驶数据应答(带油耗值)
#endif
			default:	
				PRTMSG(MSG_NOR, "Rcv unknown frm(%02x,%02x)!\n", tmp[0], tmp[1]);	
				break;
		}
	}

	// 越界报警参数设置业务
	if(0x0f == tmp[0]) 
	{
		for(ulong i=2; i<len; i++)	
		{ 
			if(0x7f==tmp[i])	
				tmp[i] = 0; 
		}
		switch(tmp[1]) 
		{
			case 0x01:	
				hdl_0f01(tmp+2, len-2);		
				break;	// 开始设置越界路线	
			case 0x02:	
				hdl_0f02(tmp+2, len-2);		
				break;	// 设置越界路线	
			case 0x03:	
				hdl_0f03(tmp+2, len-2);		
				break;	// 结束设置越界路线	
			case 0x11:	
				hdl_0f11(tmp+2, len-2);		
				break;	// 清除越界区域	
			case 0x12:	
				hdl_0f12(tmp+2, len-2);		
				break;	// 开始设置越界路线	
			default:	
				PRTMSG(MSG_NOR, "Rcv unknown frm(%02x,%02x)!\n", tmp[0], tmp[1]);	
				break;
		}
	}
}

//------------------------------------------------------------------------------	
// 实时事故疑点上传应答(0117)
void CDriveRecord::hdl_0117(byte* data, int len)
{
	byte reply = byte(data[0]);
	if( 0x01 == (reply & 0x0f) )	m_v.rp_doub[0] = reply;
	if( 0x02 == (reply & 0x0f) )	m_v.rp_doub[1] = reply;
}

//------------------------------------------------------------------------------	
// 非正式疑点上传请求(0127)
void CDriveRecord::hdl_0127(byte* data, int len)
{
	m_v.updoub = 2;		//将标志位置起即可
}

//------------------------------------------------------------------------------	
// 总里程查询请求(0901)
void CDriveRecord::hdl_0901(byte* data, int len)
{
	// 发送总里程查询请求应答
	frm0941 frm = {0};
	frm.reply = 0x01;
	memcpy(frm.time, m_v.metertime, 3);
	sprintf(frm.inf, "%u", ulong(m_v.metertotal));
	frm.len = min(127, strlen(frm.inf) );
	tcp_send(0x09, 0x41, &frm, sizeof(frm)-sizeof(frm.inf)+frm.len);

	// 将总里程写入数据文件中
	flush_headrcd();

	PRTMSG(MSG_NOR, "flush_headrcd : hdl_0901\n");
	PRTMSG(MSG_NOR, "Req total meter(%sm) succ!\n", frm.inf);
}

//------------------------------------------------------------------------------	
// 总里程清除请求(0902)
void CDriveRecord::hdl_0902(byte* data, int len)
{
	// 清除总里程和里程统计开始时间
	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);

	m_v.metertime[0] = (byte)(pCurrentTime.tm_year-100);
	m_v.metertime[1] = (byte)(pCurrentTime.tm_mon+1);
	m_v.metertime[2] = (byte)pCurrentTime.tm_mday;
	m_v.metertotal = 0;

	// 将总里程写入数据文件中
	flush_headrcd();

	byte reply = 0x01;
	tcp_send(0x09, 0x42, &reply, 1);

	PRTMSG(MSG_NOR, "flush_headrcd : hdl_0902\n");
	PRTMSG(MSG_NOR, "Clear total meter succ!\n");
}


//------------------------------------------------------------------------------	
// 查询最近360小时内和2个日历天的行驶里程请求(0904)
void CDriveRecord::hdl_0904(byte* data, int len)
{
	char str1[128] = {0};
	char str2[128] = {0};
	sprintf(str1, "%.0f", m_v.m360.get_meter());
	sprintf(str2, "%.0f", m_v.m2d.get_meter());
	byte len1 = min(126, strlen(str1));
	byte len2 = min(126, strlen(str2));

	int i = 0;
	byte frm[1024] = {0};
	frm[i++] = 0x01;		//应答类型
	frm[i++] = len1;		//里程信息长度1
	memcpy(frm+i, str1, len1);	//里程信息1
	i += len1;
	frm[i++] = len2;		//里程信息长度2
	memcpy(frm+i, str2, len2);	//里程信息2
	i += len2;
	tcp_send(0x09, 0x44, frm, i);
	PRTMSG(MSG_NOR, "Req 360(%sm, %sm) succ!\n", str1, str2);
}


//------------------------------------------------------------------------------	
// 里程记录查询(0905)
// 时间范围类型(1) + 时间范围(10) + 驾驶员身份编码(1)
void CDriveRecord::hdl_0905(byte* data, int len)
{
	ulong i=0;

	if(0==m_quemeter.getcount())
	{
		frm0905 *p = (frm0905*)data;
		RcdDrv rcd = {0};
		ulong cnt = m_v.drvcnt;
		ulong pw = m_v.drvpw;
		
		// 查找记录,将符合要求的记录存入队列中
		FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
		if(NULL!=fp) 
		{
			for(ulong i=0; i<cnt; i++) 
			{
				if(--pw>=200)	pw = 199;

				fseek(fp, offsetof(RcdFile,drv)+pw*sizeof(RcdDrv), 0);
				fread(&rcd, sizeof(rcd), 1, fp);

				if(chk_drvrcd2(&rcd, p->type, p->s_time, p->e_time, p->numdrv))
				{
					m_quemeter.push((byte*)&rcd, sizeof(rcd));
				}
			}
			fclose(fp);
		}

		if( 0 == m_quemeter.getcount() )
		{
			byte reply = 0x7f;
			tcp_send(0x09, 0x45, &reply, sizeof(reply));
		}

		PRTMSG(MSG_NOR, "Queryed %d meter(%u,%u) rcd!", m_quemeter.getcount(), p->type, p->numdrv);
	} 
	else
	{
		byte reply = 0x7f;
		tcp_send(0x09, 0x45, &reply, sizeof(reply));

		PRTMSG(MSG_NOR, "Meter is query...");
	}
}
//------------------------------------------------------------------------------	
// 设置定时上传行驶数据(103e)
// 传输数据标识(1) + 状态标识(1) + 上报总时长(2) + 采样周期(2) + 发送周期(2)
//传输数据标识：0x01: 向中心发送GPS数据   0x02: 不向中心发送GPS数据
//状态标识：    0x01  车台处于移动状态    0x02  车台处于静止状态 
//              0x03  车台处于空车状态    0x04  车台处于重车状态
void CDriveRecord::hdl_103e(byte* data, int len)
{
	frm103e *p = (frm103e*)data;

#if 1	// 屏蔽空重车状态参数
	if(1!=p->sta_id && 2!=p->sta_id)	return;
#endif

	if(p->sta_id>=1 && p->sta_id<=6) 
	{
		m_cfg.sta_id = p->sta_id;
		m_cfg.uptime[m_cfg.sta_id-1] = p->uptime;
		m_cfg.cltcyc[m_cfg.sta_id-1] = p->collect;

		if (p->send>4) 
		{
			show_diaodu("配置主动上报参数有误");
			play_tts("配置主动上报参数有误");
			PRTMSG(MSG_NOR, "CDriveRecord::hdl_103e sndcyc>4 err.\n");
			
			return;
		}
		m_cfg.sndcyc[m_cfg.sta_id-1] = p->send;
	}

	if(0x02==p->up)		m_cfg.sta_id = 0;

	// 保存配置
	uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );	

	PRTMSG(MSG_NOR, "Set updrv %s StaId=%d,CltCyc=%d,SndCyc=%d. \n", ( (1==p->sta_id) ? "AccOn" : "AccOff"),p->sta_id, p->collect, p->send);
}


//------------------------------------------------------------------------------	
// 设置车辆信息请求(3601)
// 车辆特征系数(2)+车辆VIN号长度(1)+ 车辆VIN号+
// 车辆号码长度(1)+车辆号码+车牌分类长度(1)+车牌分类
void CDriveRecord::hdl_3601(byte* data, int len)
{
	byte len2 = 0;

	memcpy(&m_cfg.factor, data, 2);
	m_cfg.factor = ntohs(m_cfg.factor);
	data += 2;

	len2 = *(data++);
	memcpy(&m_cfg.vin, data, len2 );
	data += len2;

	len2 = *(data++);
	memcpy(&m_cfg.num, data, len2 );
	data += len2;

	len2 = *(data++);
	memcpy(&m_cfg.type, data, len2 );
	data += len2;	
	
	// 保存配置
	uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );

	// 往中心发送应答
	char reply = (0==ret) ? 0x01 : 0x7f;
	tcp_send(0x36, 0x41, &reply, 1);

	PRTMSG(MSG_NOR, "Set carinfo(%u,%.17s,%.12s,%.12s) %s!\n", m_cfg.factor, 
		m_cfg.vin, m_cfg.num, m_cfg.type, (0==ret) ? "succ" : "fail");	
}


//------------------------------------------------------------------------------
// 请求设置驾驶员身份(3602)
// 设置字(1)+设置个数(1)+【序号(1)+编号(1)+
// 身份代码长度(1)+身份代码+驾驶证号长度(1)+驾驶证号】* N
void CDriveRecord::hdl_3602(byte* data, int len)
{
	byte *p = data;	
	int cnt = 0;
	int i=0;
	int iMaxAdd = 0;
	BYTE bytLen = 0;
	int ret = 0;

	DrvCfg cfg = m_cfg;

	// 设置字
	if( *p & 0x01 )
	{
		cfg.total = 0;

		for(int i=0; i<10; i++)	
			cfg.inf[i].valid = false;
	}
	p++;

	if( cfg.total > sizeof(cfg.inf) / sizeof(cfg.inf[0]) )
	{
		cfg.total = 0;
		for(int i=0; i<10; i++)	
			cfg.inf[i].valid = false;
	}

	// 设置个数
	iMaxAdd = sizeof(cfg.inf) / sizeof(cfg.inf[0]) - cfg.total;
	cnt = *p;
	if( cnt > iMaxAdd )
	{
		PRTMSG(MSG_NOR, "Set driver fail! Over Max Size!\n");
		ret = -1;
		goto HDL3602_END;
	}
	p++;

	// 设置个数  
	if(cnt>4)
	{
		PRTMSG(MSG_NOR, "Set driver fail! Set cnt err!\n");
		ret = -1;
		goto HDL3602_END;
	}
	else
	{
		PRTMSG(MSG_NOR, "Set driver cnt = %d\n", cnt);
	}

	while(cnt--)
	{
		// 获取空的记录
		int empty = -1;
		for(; i<10; i++)
		{
			if(false == cfg.inf[i].valid)
			{ 
				empty = i; 
				break; 
			}
		}

		if(-1 == empty)
		{
			PRTMSG(MSG_NOR, "Set driver fail! Cann't get empty rcd!\n");
			ret = -1; 
			goto HDL3602_END;
		}
		
		// 填充空记录

		//序号
		if(*p > 9)
		{
			PRTMSG(MSG_NOR, "Set driver fail! index err(%u)!\n",*p);
			ret = -1;
			goto HDL3602_END;
		}
		p++;

		//编号
		cfg.inf[empty].num = *p;
		if(cfg.inf[empty].num > 126)
		{
			PRTMSG(MSG_NOR, "Set driver fail! num err(%u)!\n", cfg.inf[empty].num);
			ret = -1;
			goto HDL3602_END;
		}
		p++;

		//身份代码
		bytLen = *p;
		if( bytLen > 7 )
		{
			PRTMSG(MSG_NOR, "Set driver fail! ID Len err(%u)!\n", bytLen);
			ret = -1;
			goto HDL3602_END;
		}
		p++;
		memset(cfg.inf[empty].ascii, 0, sizeof(cfg.inf[empty].ascii) );
		memcpy(cfg.inf[empty].ascii, p, bytLen );
		p += bytLen;

		//驾驶证号
		bytLen = *p;
		if( bytLen > 18 )
		{
			PRTMSG(MSG_NOR, "Set driver fail! Driver Code err(%u)!\n", bytLen);
			ret = -1;
			goto HDL3602_END;
		}
		p++;
		memset(cfg.inf[empty].code, 0, sizeof(cfg.inf[empty].code) );
		memcpy(cfg.inf[empty].code, p, bytLen );		//驾驶证号
		p += bytLen;

		cfg.inf[empty].valid = true;
		
		// 增加计数
		cfg.total++;

		// 合法性检测
		if( (p-data) > len)
		{
			PRTMSG(MSG_NOR, "Set driver fail! data err(%u)!\n");
			ret = -1;
			goto HDL3602_END;
		}
	}
	
	// 保存配置
	ret = SetSecCfg(&cfg, offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered), sizeof(cfg) );

HDL3602_END:
	// 往中心发送应答
	char reply = (0==ret) ? 0x01 : 0x7f;
	tcp_send(0x36, 0x42, &reply, 1);

	if(0==ret)
	{
		m_cfg = cfg;
		PRTMSG(MSG_NOR, "Set driver %s", (0==cnt) ? "finished!" : "...\n");
	}
	else
	{
		PRTMSG(MSG_NOR, "Set driver fail!\n");
	}
}


//------------------------------------------------------------------------------
// 设置疲劳行驶时间(3603)
// 疲劳行驶时间(1)+司机休息间隔时间(1)
// 疲劳行驶时间：单位为小时，如为0，则表示不进行疲劳行驶检测
// 司机休息间隔时间：单位为分钟
void CDriveRecord::hdl_3603(byte* data, int len)
{
	m_cfg.max_tire = *data++;
	m_cfg.min_sleep = *data++;
	
	// 保存配置
	uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );

	// 往中心发送应答
	char reply = (0==ret) ? 0x01 : 0x7f;
	tcp_send(0x36, 0x43, &reply, 1);

	PRTMSG(MSG_NOR, "Set tire time(%u,%u) %s!\n", m_cfg.max_tire, m_cfg.min_sleep, (0==ret) ? "succ" : "fail");
}


//------------------------------------------------------------------------------
// 查询事故疑点数据请求(3604)
// 查询字(1)+查询时间段个数(1)+【查询时间段】+查询范围个数(1)+【位置范围】
// 查询时间段：起点时间(5)+终点时间(5)
// 位置范围：左下角经度、纬度(8)+右上角经度、纬度(8)
void CDriveRecord::hdl_3604(byte* data, int len)
{
	if(0==m_quedoub.getcount())
	{
		// 解析协议
		byte attrib = *data++;
		
		byte timecnt = *data++;
		if(timecnt>10)		return;
		byte time[10][10] = {0};
		memcpy(time, data, timecnt*10);
		data += timecnt*10;
		
		byte regioncnt = *data++;
		if(regioncnt>5)		return;
		byte region[5][16] = {0};
		memcpy(region, data, regioncnt*16);
		data += regioncnt*16;
			
		// 查找记录,将符合要求的记录存入队列中
		RcdDoub rcd = {0};
		ulong cnt = m_v.doubcnt;
		ulong pw = m_v.doubpw;	
		
		FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
		if(NULL!=fp)
		{
			for(ulong i=0; i<cnt; i++)
			{
				if(--pw >100)		pw = 99;

				fseek(fp, offsetof(RcdFile,doub)+pw*sizeof(RcdDoub), 0);
				fread(&rcd, sizeof(rcd), 1, fp);
				
				if(chk_doubrcd(&rcd,timecnt,time,regioncnt,region))
				{
					m_quedoub.push((byte*)&rcd, sizeof(rcd));
				}
			}
			fclose(fp);
		}	
		
		if( (0==m_quedoub.getcount()) && (attrib & 0x01) )
		{
			byte reply = 0x7f;
			tcp_send(0x36, 0x44, &reply , sizeof(reply) );
		}

		PRTMSG(MSG_NOR, "Queryed %d doub(%u,%u) rcd!\n", m_quedoub.getcount(), timecnt, regioncnt);
	}
	else 
	{
		byte reply = 0x7f;
		tcp_send(0x36, 0x44, &reply , sizeof(reply) );
		PRTMSG(MSG_NOR, "Doub is query...!\n");
	}
}


//------------------------------------------------------------------------------
// 查询最近15分钟内每分钟平均速度请求(3605)
void CDriveRecord::hdl_3605(byte* data, int len)
{
	frm3645 frm = {0};
	frm.reply = 0x01;
	memcpy(frm.average, m_v.average, 15);
	
	tcp_send(0x36, 0x45, &frm, sizeof(frm) );	
	
	PRTMSG(MSG_NOR, "Query 15speed succ!\n");
}


//------------------------------------------------------------------------------
// 查询连续行驶时间记录请求(3606)
// 司机编号(1)+连续行驶时间(1)
// 司机编号：如为0，则表示查询所有司机编号
// 连续行驶时间：单位为小时
void CDriveRecord::hdl_3606(byte* data, int len)
{
	if( 0 == m_quedrv.getcount() )
	{
		frm3606 *p = (frm3606*)data;
		RcdDrv rcd = {0};
		ulong cnt = m_v.drvcnt;
		ulong pw = m_v.drvpw;
		
		// 查找记录,将符合要求的记录存入队列中
		FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
		if(NULL!=fp) 
		{
			for(ulong i=0; i<cnt; i++) 
			{
				if(--pw>=200)		pw = 199;

				fseek(fp, offsetof(RcdFile,drv)+pw*sizeof(RcdDrv), 0);
				fread(&rcd, sizeof(rcd), 1, fp);

				if(chk_drvrcd(&rcd,p->numdrv,p->times))
				{
					m_quedrv.push((byte*)&rcd, sizeof(rcd));
				}
			}

			fclose(fp);
		}

		if( 0 == m_quedrv.getcount() ) 
		{
			byte reply = 0x7f;
			tcp_send(0x36, 0x46, &reply, sizeof(reply));
		}

		PRTMSG(MSG_NOR, "Queryed %d drv(num:%u,%uh) rcd!\n", m_quedrv.getcount(), p->numdrv, p->times);		
	} 
	else 
	{
		byte reply = 0x7f;
		tcp_send(0x36, 0x46, &reply, sizeof(reply));
		PRTMSG(MSG_NOR, "Drv is query...\n");
	}	
}


//------------------------------------------------------------------------------
// 设置超速参数
// 上限速度(1) + 超速计量时间(1)
// 上限速度：单位为海里/小时，速度如为0，则表示不进行超速判断
// 超速计量时间：单位为秒；表示超速持续超过这个时间，就判断为超速。
void CDriveRecord::hdl_3611(byte* data, int len)
{
	m_cfg.max_speed = *data++;
	m_cfg.min_over = *data++;
	
	// 保存配置
	uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );

	// 往中心发送应答
	char reply = (0==ret) ? 0x01 : 0x7f;
	tcp_send(0x36, 0x51, &reply, 1);

	PRTMSG(MSG_NOR, "Set overspeed(%usm/h,%us) %s!\n", m_cfg.max_speed, 
		m_cfg.min_over, (0==ret) ? "succ" : "fail");
}


//------------------------------------------------------------------------------
// 设置低速参数
// 下限速度(1) + 低速计量时间(1)
// 下限速度：单位为公里/小时，速度如为255，则表示不进行低速判断
// 低速计量时间：单位为分钟；表示低速持续超过这个时间，就判断为低速。
void CDriveRecord::hdl_3619(byte* data, int len)
{
	m_cfg.min_speed = *data++;
	m_cfg.min_below = *data++;
	
	// 保存配置
	uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );

	// 往中心发送应答
	char reply = (0==ret) ? 0x01 : 0x7f;
	tcp_send(0x36, 0x59, &reply, 1);

	PRTMSG(MSG_NOR, "Set belowspeed(%ukm/h,%um) %s!\n", m_cfg.min_speed, m_cfg.min_below, (0==ret) ? "succ" : "fail");
}

//------------------------------------------------------------------------------
// 设置扩展参数
// 当前版本号(1B) +【扩展类型(2B) + 长度(2B) + 数据】
//		当前版本号(1B)：当前版本号为1。
//		扩展类型：
//				0x0001--设置车台怠速速度：
//				上限速度(1B)：单位为海里/小时；
//				持续时间(2B)：单位为分钟，高字节在前；持续低速超过这个时间，就判断为怠速。上限速度为0表示停止怠速报警。

void CDriveRecord::hdl_3623(byte* data, int len)
{
	char	szVer = 0;
	char	sztmpbuf[2] = {0};
	short	shType = 0;

	char	szRes = 1;
	char	szReplyBuf[200] = {0};
	int		iReplyLen = 0;

	// 版本号
	szVer = (char)data[0];

	for(int i=1; i<len; )
	{
		// 获取扩展类型
		sztmpbuf[0] = (char)data[i+1];
		sztmpbuf[1] = (char)data[i];
		i += 2;

		memcpy((void*)&shType, sztmpbuf, 2);

		switch( shType )
		{
		case 0x0001:	// 低速报警设置
			{
				//检查参数长度
				if( len != 8 )
				{
					szRes = 0;
					goto _DEAL_3623_END;
				}

				// 跳过长度字段
				i += 2;

				//上限速度
				m_cfg.min_speed = byte(data[i]*1.852);
				i += 1;

				// 持续时间
				m_cfg.min_below = 256*data[i] + data[i+1];
				i += 2;

				// 保存配置
				uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
				int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );
			}
			break;

		case 0x0002:	// 电子围栏设置
		case 0x0003:	// 超市停车时间设置
		default:
			break;
		}
	}

_DEAL_3623_END:
	// 给中心应答
	szReplyBuf[iReplyLen++] = szVer;		// 版本号
	szReplyBuf[iReplyLen++] = sztmpbuf[0];	// 扩展类型
	szReplyBuf[iReplyLen++] = sztmpbuf[1];
	szReplyBuf[iReplyLen++] = szRes;		// 应答类型
	switch( shType )
	{
	case 0x0001:
		szReplyBuf[iReplyLen++] = 0;		// 扩展信息附加长度为0
		break;

	case 0x0002:
	case 0x0003:
	default:
		break;
	}

	tcp_send(0x36, 0x63, szReplyBuf, iReplyLen);
	PRTMSG(MSG_NOR, "Set 3623 belowspeed(%ukm/h,%um) %s!\n", m_cfg.min_speed, m_cfg.min_below, (1==szRes) ? "succ" : "fail");
}

//------------------------------------------------------------------------------
// 采集司机信息
void CDriveRecord::hdl_3613(byte* data, int len)
{
	int i = 0;
	byte frm[1024] = {0};

	int index = get_drvindex(m_v.numdrv);
	
	frm[i++] = byte(m_v.numdrv);	//司机编号
	
	if(-1 != index)
	{
		int asclen = min(7, strlen(m_cfg.inf[index].ascii) );
		frm[i++] = asclen;		//身份代码长度
		memcpy(frm+i, m_cfg.inf[index].ascii, asclen);	//身份代码
		i += asclen;

		int codlen = min(18, strlen(m_cfg.inf[index].code) );
		frm[i++] = codlen;		//驾驶证号长度
		memcpy(frm+i, m_cfg.inf[index].code, codlen);	//驾驶证号
		i += codlen;
	}
	else 
	{
		frm[i++] = 0;		//身份代码长度
		frm[i++] = 0;		//驾驶证号长度
	}

	tcp_send(0x36, 0x53, frm, i);

	PRTMSG(MSG_NOR, "Collect driver info %s(idx:%d)!\n",(-1!=index) ? "succ" : "fail", index);
}

void CDriveRecord::hdl_3618(byte* data, int len)
{
// 窗口大小（1）+ ACCON采样周期（2）+ ACCON发送周期（1）+ ACCOFF采样周期（2）+ ACCOFF发送周期（1）

	BYTE bytResType = 0x01;
	uint offset = 0;
	int ret = 0;

	BYTE bytWinSize = data[0];
	if( bytWinSize > 32 )
	{
		bytResType = 0x7f;
		goto HDL3618_END;
	}

	if( 0x7f == data[1] ) data[1] = 0;
	if( 0x7f == data[2] ) data[2] = 0;
	m_cfg.cltcyc[0] = data[1] * 60 + data[2];

	if( 0x7f == data[3] ) data[3] = 0;
	m_cfg.sndcyc[0] = data[3];

	m_cfg.uptime[0] = 0xffff;

	if( 0x7f == data[4] ) data[4] = 0;
	if( 0x7f == data[5] ) data[5] = 0;
	m_cfg.cltcyc[1] = data[4] * 60 + data[5];
	
	if( 0x7f == data[3] ) data[3] = 0;
	m_cfg.sndcyc[1] = data[6];

	m_cfg.uptime[1] = 0xffff;

	m_cfg.sta_id = ( 1 == m_cfg.sta_id) ? 2 : 1;
	
	// 保存配置
	offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
	ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );
	if( ret ) bytResType = 0x7f;
	
HDL3618_END:
	tcp_send(0x36, 0x58, &bytResType, 1);
}

//------------------------------------------------------------------------------
// 设置行驶时间段请求(3701)
// 设置字(1) + 设置个数(1) + 【时间段编号(1) + 时间段(4)】
void CDriveRecord::hdl_3701(byte* data, int len)
{
	frm3701 *p = (frm3701*)data;

	// 清除所有时间段设置
	if(p->ctl & 0x01)	memset(m_v.almtime, 0, sizeof(m_v.almtime));

	for(int i=0; i<min(20,p->cnt); i++) 
	{
		byte no = min(19, p->time[i][0]);	//时间段编号
		memcpy(&m_v.almtime[no][0], &p->time[i][1], 4);
	}

	byte reply = flush_headrcd() ? 0x01 : 0x7f;
	tcp_send(0x37, 0x41, &reply, 1);

	PRTMSG(MSG_NOR, "Set drvtime(%u) %s!\n", p->cnt, (0x01==reply) ? "succ" : "fail");
}



//------------------------------------------------------------------------------
// 设置报警区域请求(3702)
// 帧序号(2) + 设置字(1) + 【区域编号(2) + 区域属性(1) + 上限速度(1) + 区域点个数(2) + 
// 【区域点经度(4) + 区域点纬度(4)】】
void CDriveRecord::hdl_3702(byte* data, int len)
{
	static bool setting = false;
	static ushort nextno = 0;
	static int lstcnt = 0;
	static ulong tm_set = GetTickCount();
	static Range rg = {0};
	
	frm37021 *p = (frm37021*)data;
	bool succ = false;
	
	while(1) 
	{
		// 判断是否处于设置状态(设置超时则退出设置状态)
		if(setting && (GetTickCount()-tm_set)<10*60*1000) 
		{
			if(nextno!=ntohs(p->frmno))	
			{
				PRTMSG(MSG_ERR, "p->frmno(%u)!=0\n", ntohs(p->frmno)); 
				break; 
			}
			nextno++;	
		} 
		else
		{
			if(0!=ntohs(p->frmno))	
			{ 
				PRTMSG(MSG_ERR, "p->frmno(%u)!=nextno(%u)\n", ntohs(p->frmno), nextno);
				break;
			}
			nextno = 1;
			setting = true;
			tm_set = GetTickCount();
		}
		
		// 帧号为0,并且设置字为清除,则清除所有区域记录
		if((p->ctl&0x02) && (ntohs(p->frmno)==0) ) 
		{ 
			m_v.r_used = 0; 
			m_v.d_used = 0;
			flush_headrcd();
			PRTMSG(MSG_ERR, "Clear all area rcd!\n"); 
		}
		
		// 判断设置结束标志
		if(p->ctl & 0x01)
		{ 
			setting = false;
			succ = true; 
			break; 
		}
		
		// 解析区域属性
		if( 0==lstcnt ) 
		{
			rg.areano = ntohs(p->areano);
			rg.attr = p->attr;
			rg.vector = p->vector;
			rg.cnt = ntohs(p->cnt);
			rg.n_start = m_v.d_used;
			rg.n_end = rg.n_start + rg.cnt;

			if(m_v.r_used>=200)	//块区域是否用完
			{ 
				PRTMSG(MSG_ERR, "m_v.r_used(%u)>=200", m_v.r_used);
				break; 
			}	

			if(rg.cnt>200)		//设置个数是否太大
			{
				PRTMSG(MSG_ERR, "rg.cnt(%u)>200", rg.cnt); 
				break; }	
			
			if(rg.n_end>=800)	//点区域是否不够用
			{
				PRTMSG(MSG_ERR, "rg.n_end(%u)>=800", rg.n_end);
				break; 
			}		
		} 
		
		// 解析并存储区域点
		FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
		if(NULL!=fp)
		{
			if(0==lstcnt)
				lstcnt = rg.cnt;

			for(int i=0; i<=lstcnt; i++) //存储区域点
			{	
				int offset = (lstcnt==rg.cnt) ? offsetof(frm37021,dot)+i*8 : offsetof(frm37022,dot)+i*8 ;
				if(offset>=len)
				{ 
					lstcnt = lstcnt - i;
					break;
				}

				fseek(fp, offsetof(RcdFile, dot)+(rg.n_start+rg.cnt-lstcnt+i)*8, 0);
				fwrite(data+offset, 1, 8, fp);
			}

			if(0==lstcnt)	//存储区域属性
			{		
				fseek(fp, offsetof(RcdFile, range)+m_v.r_used*sizeof(Range), 0);
				fwrite(&rg, 1, sizeof(rg), fp);
			
				m_v.d_used = rg.n_end;
				if(++m_v.r_used>=200)	m_v.r_used = 199;

				fseek(fp, offsetof(RcdFile, head), 0);
				fwrite(&m_v, 1, sizeof(m_v), fp);
			
				PRTMSG(MSG_ERR, "save area%u(%u-%u) data!\n", m_v.r_used, rg.n_start, rg.n_end);
			}
			fclose(fp);
		} 
		else 
		{
			break;
		}
		
		succ = true;
		break;		//此处while不做循环,故需马上退出
	}
	
	// 结束设置状态
	if(!succ || !setting)
	{
		setting = false;
		nextno = 0;
		lstcnt = 0;	
		tm_set = GetTickCount();
		PRTMSG(MSG_ERR, "set alarm area %s!\n", (!succ) ? "fail" : "finish");
	}
	
	// 发送设置报警区域请求应答
	frm3742 frm = {0};
	frm.reply = succ ? 0x01 : 0x7f;
	frm.frmno = p->frmno;
	tcp_send(0x037, 0x42, &frm, succ ? 1 : 2 );
}


//------------------------------------------------------------------------------
// 请求上传行驶数据应答(带油耗值)
// 应答类型(1) + 帧序号掩码(4)
void CDriveRecord::hdl_3707(byte* data, int len)
{
	if( 0x01 == *data )
	{
		m_v.rp_oil[0] = *data;	
	} 
	else
	{
		memcpy(m_v.rp_oil, data, 5);
	}
}

void CDriveRecord::hdl_3709(byte *data, int len)
{
	if( 0x01 == *data )
	{
		m_v.rp_oil[0] = *data;	
	} 
	else
	{
		memcpy(m_v.rp_oil, data, 5);
	}
}

//------------------------------------------------------------------------------
// 开始设置越界路线
// 设置标志（1）
void CDriveRecord::hdl_0f01(byte* data, int len)
{

}


//------------------------------------------------------------------------------
// 设置越界路线
// 区域个数（1）+ [越界路线区域1 + …… + 越界路线区域n]
void CDriveRecord::hdl_0f02(byte* data, int len)
{

}

//------------------------------------------------------------------------------
// 结束设置越界路线
// 总共设置区域个数（1）
void CDriveRecord::hdl_0f03(byte* data, int len)
{

}


//------------------------------------------------------------------------------
// 清除越界区域
// 无
void CDriveRecord::hdl_0f11(byte* data, int len)
{
	tag2QAreaCfg objAreaCfg;
	int iret;
	byte bytRes = 0x7f;

	objAreaCfg.Init();

	// 写入配置
	iret = SetSecCfg((void*)&objAreaCfg, offsetof(tagSecondCfg, m_uni2QAreaCfg.m_obj2QAreaCfg), sizeof(objAreaCfg));
	if( !iret )
	{
		bytRes = 0x01;
		memset((void*)&m_objAreaCfg, 0, sizeof(m_objAreaCfg));
	}

	
	tcp_send(0x0f, 0x51, &bytRes, 1);
}


//------------------------------------------------------------------------------
// 设置越界区域
// 区域个数（1） + [越界区域1 + …… + 越界区域n]
/*
区域属性	1	字节格式定义：01NNNNAA
			NNNN：区域编号，取值范围0000~1111
			AA：定位属性
			00-区域内定位（逻辑与）
			01-区域内定位（逻辑或）
			10-区域外定位（逻辑与）
			11-区域外定位（逻辑或）

Hbit1		1	字节格式定义：01GDDDDD
			G：存放参考点经度度分量最高位
			DDDDD：存放经度方向的区域宽度的度分量，取值范围：00000~11111

经度方向
区域宽度	3	整数分（1）+ 小数分（2）

Hbit2		1	字节格式定义为：010DDDDD
			DDDDD：存放纬度方向的区域宽度的度分量，取值范围：00000~11111
纬度方向
区域宽度	3	整数分（1）+ 小数分（2）

参考点经、纬度	8	参考点经度（4）+ 参考点纬度（4）

备注：参考点取区域的左下角，参考点经、纬度参见标准GPS数据中的经、纬度定义。
备注：整数分、小数分定义可参见标准GPS数据中的经、纬度定义。
*/
void CDriveRecord::hdl_0f12(byte* data, int len)
{
	tag2QAreaCfg objAreaCfg;
	tag0f12Area objOneArea;
	byte bytAreaCnt = 0;
	int  iret = 0;
	byte bytRes = 0x7f;		// 默认为失败应答

	objAreaCfg.Init();
	bytAreaCnt = data[0];

	// 范围个数1～6
	if( bytAreaCnt > 6 || bytAreaCnt < 1 )
	{
		PRTMSG(MSG_ERR, "Deal0f12 err, Area Cnt uncorrect!\n");
		goto _DEAL0F12_END;
	}

	// 参数长度检查
	if( len < sizeof(objOneArea)*bytAreaCnt + 1 )
	{
		PRTMSG(MSG_ERR, "Deal0f12 err, data len uncorrect!\n");
		goto _DEAL0F12_END;
	}

	for(int i=0; i<bytAreaCnt; i++)
	{
		double dTemp = 0.0;
		BYTE bytTemp = 0;

		memcpy( (void*)&objOneArea, data+1+i*sizeof(objOneArea), sizeof(objOneArea) );

		objAreaCfg.m_aryAreaProCode[i] = objOneArea.m_bytAreaProCode;
				
		// 左下经度
		bytTemp = (0x20 & objOneArea.m_bytLonMix ? 0x80 : 0) + objOneArea.m_bytLonDu;
		if( 0xff == bytTemp ) bytTemp = 0;
		else if( 0xfe == bytTemp ) bytTemp = 0x80;
		if( 0x7f == objOneArea.m_bytLonFenzen ) objOneArea.m_bytLonFenzen = 0;
		if( 0x7f == objOneArea.m_bytLonFenxiao1 ) objOneArea.m_bytLonFenxiao1 = 0;
		if( 0x7f == objOneArea.m_bytLonFenxiao2 ) objOneArea.m_bytLonFenxiao2 = 0;
		dTemp = objOneArea.m_bytLonFenzen + objOneArea.m_bytLonFenxiao1 * 0.01 + objOneArea.m_bytLonFenxiao2 * 0.0001;
		objAreaCfg.m_aryArea[ i ][ 0 ] = long( (bytTemp + dTemp / 60) * LONLAT_DO_TO_L );
				
		// 左下纬度
		bytTemp = objOneArea.m_bytLatDu;
		if( 0x7f == bytTemp ) bytTemp = 0;
		if( 0x7f == objOneArea.m_bytLatFenzen ) objOneArea.m_bytLatFenzen = 0;
		if( 0x7f == objOneArea.m_bytLatFenxiao1 ) objOneArea.m_bytLatFenxiao1 = 0;
		if( 0x7f == objOneArea.m_bytLatFenxiao2 ) objOneArea.m_bytLatFenxiao2 = 0;
		dTemp = objOneArea.m_bytLatFenzen + objOneArea.m_bytLatFenxiao1 * 0.01 + objOneArea.m_bytLatFenxiao2 * 0.0001;
		objAreaCfg.m_aryArea[ i ][ 1 ] = long( (bytTemp + dTemp / 60) * LONLAT_DO_TO_L );
		
		// 右上经度
		if( 0x7f == objOneArea.m_bytLonWidFenZhen ) objOneArea.m_bytLonWidFenZhen = 0;
		if( 0x7f == objOneArea.m_bytLonWidFenxiao1 ) objOneArea.m_bytLonWidFenxiao1 = 0;
		if( 0x7f == objOneArea.m_bytLonWidFenxiao2 ) objOneArea.m_bytLonWidFenxiao2 = 0;
		bytTemp = objOneArea.m_bytLonMix & 0x1f;
		dTemp = objOneArea.m_bytLonWidFenZhen + objOneArea.m_bytLonWidFenxiao1 * 0.01 + objOneArea.m_bytLonWidFenxiao2 * 0.0001;
		objAreaCfg.m_aryArea[ i ][ 2 ] = objAreaCfg.m_aryArea[ i ][ 0 ] + long( (bytTemp + dTemp / 60) * LONLAT_DO_TO_L );
		
		// 右上纬度
		bytTemp = objOneArea.m_bytLatMix & 0x1f;
		if( 0x7f == objOneArea.m_bytLatHeiFenZhen ) objOneArea.m_bytLatHeiFenZhen = 0;
		if( 0x7f == objOneArea.m_bytLatHeiFenxiao1 ) objOneArea.m_bytLatHeiFenxiao1 = 0;
		if( 0x7f == objOneArea.m_bytLatHeiFenxiao2 ) objOneArea.m_bytLatHeiFenxiao2 = 0;
		dTemp = objOneArea.m_bytLatHeiFenZhen + objOneArea.m_bytLatHeiFenxiao1 * 0.01 + objOneArea.m_bytLatHeiFenxiao2 * 0.0001;
		objAreaCfg.m_aryArea[ i ][ 3 ] = objAreaCfg.m_aryArea[ i ][ 1 ] + long( (bytTemp + dTemp / 60) * LONLAT_DO_TO_L );

		objAreaCfg.m_bytAreaCount ++;

		PRTMSG(MSG_DBG, "i=%d, %ld, %ld, %ld, %ld\n", i, objAreaCfg.m_aryArea[i][0], objAreaCfg.m_aryArea[i][1],objAreaCfg.m_aryArea[i][2], objAreaCfg.m_aryArea[i][3]);
	}

	// 写入配置
	iret = SetSecCfg((void*)&objAreaCfg, offsetof(tagSecondCfg, m_uni2QAreaCfg.m_obj2QAreaCfg), sizeof(objAreaCfg));
	if( !iret )
	{
		bytRes = 0x01;

		// 启用新的设置
		memset((void*)&m_objAreaCfg, 0, sizeof(m_objAreaCfg));
		memcpy((void*)&m_objAreaCfg, (void*)&objAreaCfg, sizeof(objAreaCfg));
	}

_DEAL0F12_END:
	tcp_send(0x0f, 0x52, &bytRes, 1);
}


//------------------------------------------------------------------------------
// 处理调度屏行车记录仪的协议
void CDriveRecord::hdl_diaodu(byte* data, int len)
{
	char szbuf[100] = {0};
	szbuf[0] = 0x01;		// 使用了串口扩展盒，表示要发送到调度屏的数据

	switch(*data) 
	{
	case 0x61: // 请求速度
		{
			frm60 frm;
			frm.cmd = 0x60;
			frm.reply = 0x01;
			frm.speed = m_v.vector[0];	//ACC无效则将速度设为0

			memcpy(szbuf+1, (char*)&frm, sizeof(frm));
			DataPush(szbuf, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
		}
		break;

	case 0x63: // 请求司机登录应答
		{
			frm63 *p = (frm63*)data;

			if(0x01 == p->reply)	
			{
				m_v.inputflag = htons(p->numdrv);
				PRTMSG(MSG_NOR, "DrvLog m_v.inputflag=%d.\n",	m_v.inputflag);
			}
			else 
				m_v.inputflag = -1;
		}
		break;

	case 0x65: // 请求司机编号
		{
			frm64 frm = {0};
			frm.cmd = 0x64;
			frm.reply = 0x01;
			frm.numdrv = htons(ushort(m_v.numdrv));
			int index = get_drvindex(m_v.numdrv);
			frm.asclen = 0;
			if(-1 != index) 
			{
				frm.asclen = min(7, strlen(m_cfg.inf[index].ascii) );
				memcpy(frm.ascii, m_cfg.inf[index].ascii, frm.asclen);
			}

			memcpy(szbuf+1, (char*)&frm, sizeof(frm));
			DataPush(szbuf, sizeof(frm)+1, DEV_QIAN, DEV_DIAODU, LV2);
		}
		break;

	case 0x67: // 请求载重传感器数据
		break;

	case 0x69: // 请求传感器状态
		break;

	case 0x6B: // 请求文本信息显示应答
		break;

	case 0x6C: // 司机登出指示应答
		break;

	case 0x6D: // 车辆特征系数指令
		{
			int i = 0;
			byte frm[10] = {0};
			static ulong pulse = 0;
			frm[i++] = 0x6E;		//命令类型
			frm[i++] = *(data+1);	//指令号

			switch(*(data+1))
			{
			case 0x01:	//查询
				{
					PRTMSG(MSG_NOR, "query factor = %u\n", m_cfg.factor);
					
					ushort factor = htons(m_cfg.factor);
					frm[i++] = 0x01;	//应答类型
					memcpy(frm+i, &factor, 2);	i += 2;
				}
				break;

			case 0x02:	//设置
				{	
					ushort factor = 0;
					memcpy(&factor, data+2, 2);

					PRTMSG(MSG_NOR, "set factor = %u", htons(factor));
				
					m_cfg.factor = htons(factor);
					uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
					int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );
				
					frm[i++] = 0x01;	//应答类型
				}
				break;
				
			case 0x03:	//启动检测
				{
					g_objQianIO.IN_QueryMeterCycle(pulse);
					PRTMSG(MSG_NOR, "Begin chk pulse(%u)..\n", pulse);
					frm[i++] = 0x01;	//应答类型
				}
				break;

			case 0x04:	//停止检测
				{
					ulong pulse2 = 0;
					g_objQianIO.IN_QueryMeterCycle(pulse2);

					PRTMSG(MSG_NOR, "End chk pulse(%u)!", pulse);

					pulse = pulse2 - pulse;
					frm[i++] = 0x01;	//应答类型
				}
				break;

			case 0x05:	//用户输入里程
				{
					byte mile = *(data+2);	//用户输入里程数(公里)
				
					//if(mile>0 && mile<=255)	// byte型不用这个判断吧
					{
						m_cfg.factor = ushort(pulse / mile);
						uint offset = offsetof(tagSecondCfg, m_uniDrvCfg.m_szReservered);
						int ret = SetSecCfg(&m_cfg, offset, sizeof(m_cfg) );
						
						frm[i++] = 0x01;	//应答类型
					} 
// 					else 
// 					{
// 						frm[i++] = 0x02;	//应答类型
// 					}

					PRTMSG(MSG_NOR, "Result is %u=%u/%u\n", m_cfg.factor, pulse, mile);
				}
				break;
				
			default:
				break;
			}

			memcpy(szbuf+1, (char*)frm, i);
			DataPush(szbuf, i+1, DEV_QIAN, DEV_DIAODU, LV2);
		}
		break;

	case 0x6F: // 总里程相关指令
		{
			int i = 0;
			byte frm[10] = {0};
			frm[i++] = 0x6F;		//命令类型
			frm[i++] = *(data+1);	//指令号

			switch(*(data+1))
			{
			case 0x01:	//查询
				{
					PRTMSG(MSG_NOR, "query total = %u\n", ulong(m_v.metertotal));
					
					ulong total = htonl(ulong(m_v.metertotal));
					frm[i++] = 0x01;	//应答类型
					memcpy(frm+i, &total, 4);	i += 4;
				}
				break;

			case 0x02:	//清0
				{
					m_v.metertotal = 0;
					flush_headrcd();
					frm[i++] = 0x01;	//应答类型
				}
				break;

			case 0x03:	//设置
				{
					ulong total = 0;
					memcpy(&total, data+2, 4);
					PRTMSG(MSG_NOR, "set total = %u\n", htonl(total));
					
					m_v.metertotal = htonl(total);
					flush_headrcd();
					frm[i++] = 0x01;	//应答类型
				}
				break;
				
			default:
				break;
			}

			memcpy(szbuf+1, (char*)frm, i);
			DataPush(szbuf, i+1, DEV_QIAN, DEV_DIAODU, LV2);
		}
		break;

	default:
		PRTMSG(MSG_NOR, "Rcv unknown diaodu(%02X)!\n", *data);
		break;
	}
}

//------------------------------------------------------------------------------
// 从记录文件中读出最后一条未上传的事故疑点
// updoub : 事故疑点类型,1正式事故疑点类型 2全部疑点类型
// rd: 传入的事故疑点结构指针
// 返回: 该记录在文件中的位置(0为没有找到记录)
ulong CDriveRecord::get_lstdoub(byte updoub, RcdDoub &rd)
{
	RcdDoub rcd;
	ulong cnt = m_v.doubcnt;
	ulong pw = m_v.doubpw;
	ulong find = 0;
	
	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
	if(NULL!=fp)
	{
		for(ulong i=0; i<cnt; i++) 
		{
			if(--pw >100)		pw = 99;

			fseek(fp, offsetof(RcdFile,doub)+pw*sizeof(RcdDoub), 0);
			fread(&rcd, sizeof(rcd), 1, fp);

			if( 0 == (rcd.stoptype & 0x80) ) //该记录还未上报过
			{	
				if( (1==updoub && 0x02==rcd.stoptype) || 2==updoub )	//正式疑点 || 全部疑点
					find = pw + 1;
			}

			if(find)
			{ 
				rd = rcd;
				break;
			}
		}

		fclose(fp);
	}
	return find;
}


//------------------------------------------------------------------------------
// 将记录文件内事故疑点的上报属性设置为已上报
// pw: 记录的指针
void CDriveRecord::set_doubtype(ulong pw)
{
	RcdDoub rcd;
	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");

	if(NULL!=fp) 
	{
		fseek(fp, offsetof(RcdFile,doub)+pw*sizeof(RcdDoub), 0);
		fread(&rcd, sizeof(rcd), 1, fp);
		rcd.stoptype = rcd.stoptype | 0x80;
		
		fseek(fp, offsetof(RcdFile,doub)+pw*sizeof(RcdDoub), 0);
		fwrite(&rcd, sizeof(rcd), 1, fp);
		fclose(fp);
	}
}


//------------------------------------------------------------------------------
// 填充3747或3749协议帧
#if USE_OIL == 3
void CDriveRecord::fill_frm3749(frm3749 &frm, int c_snd, ushort cltcyc, ushort sndcyc, tagGPSData gps)
{
	if(0==c_snd) {	//采集第一个GPS点时
		frm.reply = 0;
		frm.w_size = m_cfg.winsize;
		frm.frmno = 0;
		frm.tm_spc = htons(cltcyc);
		frm.ct_gps = byte(sndcyc);

		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);

		frm.date[0]	= (byte)(pCurrentTime.tm_year-100);
		frm.date[1]	= (byte)(pCurrentTime.tm_mon+1);
		frm.date[2]	= (byte)pCurrentTime.tm_mday;
		frm.time[0]	= (byte)pCurrentTime.tm_hour;
		frm.time[1]	= (byte)pCurrentTime.tm_min;
		frm.time[2] = (byte)pCurrentTime.tm_sec;


		BYTE bytKuozhan = 0;
		if(gps.valid == 'A') bytKuozhan |= 0x01;
		if(g_objOil.bConnect()) bytKuozhan |= 0x10;  //带油耗数据 
		bytKuozhan |= 0x06;  //带报警数据和行驶记录仪数据
		
		bytKuozhan = 0x1f;	// 带GPS数据、行驶记录数据、报警状态数据、载重数据、油耗数据
		frm.m_bytKuoZhanSta1 = bytKuozhan;
		frm.m_bytKuoZhanSta2 = 0;
	}
	frm.inf[c_snd].numdrv = htons(ushort(m_v.numdrv));

	double db = 60*(gps.latitude-byte(gps.latitude));
	frm.inf[c_snd].lat[0] = byte(gps.latitude);
	frm.inf[c_snd].lat[1] = (byte)db;
	frm.inf[c_snd].lat[2] = byte(long(db*100)%100);
	frm.inf[c_snd].lat[3] = byte(long(db*10000)%100);
	db = 60*(gps.longitude-byte(gps.longitude));
	frm.inf[c_snd].lon[0] = byte(gps.longitude);
	frm.inf[c_snd].lon[1] = (byte)db;
	frm.inf[c_snd].lon[2] = byte(long(db*100)%100);
	frm.inf[c_snd].lon[3] = byte(long(db*10000)%100);

	frm.inf[c_snd].speed = m_v.vector[0];
	frm.inf[c_snd].dir = byte(gps.direction/3) + 1;
	
	{
		DWORD dwIoSta = 0;
		// 获取传感器信息
		g_objQianIO.IN_QueryIOSta( dwIoSta );
		// 协议定义: D7～D0：其中0表示无信号，1表示有信号
		//	D7--脚刹信号
		//  D6--手刹信号
		//  D5--左转向灯信号
		//  D4--右转向灯信号
		//  D3--前车门信号
		//  D2--后车门信号
		//  D1--电子喇叭信号
		//  D0--ACC信号
		frm.inf[c_snd].sensor = BYTE(dwIoSta % 0x100);

		DWORD dwVeSta = g_objCarSta.GetVeSta();
		if(dwVeSta & VESTA_HEAVY)//重车
		{
			frm.inf[c_snd].sensor |= 0x02;
		}
		else//空车
		{
			frm.inf[c_snd].sensor &= 0xFD;
		}
		// 获取状态信息
		
		// 协议定义:
		// 状态字：H7 H6 H5 H4 H3 H2 H1 H0
		//		   L7 L6 L5 L4 L3 L2 L1 L0
		// H0：GPRS在线情况  0不在线        1在线
		// H1：发生碰撞       0 无碰撞       1 碰撞
		// H2：发生侧翻       0无侧翻       1 侧翻
		// H3：欠压状态       0 无欠压       1欠压
		// H4：主电断电状态   0 主电未断电   1主电断电
		// H5：抢劫报警状态   0无抢劫报警   1发生抢劫报警
		// H6：非法启动状态   0 无非法启动   1 发生非法启动
		// H7：保留
		// 
		// L1、L0经纬度坐标:
		// 0  东经北纬  1  东经南纬  2  西经北纬  3  西经南纬
		// L3、L2 GPS模块定位状态：
		// 0：未定位    1：2D定位    2：3D定位   3 保留
		// L5、L4 GPS模块工作状态：
		// 0：正常      1：省电       2：通讯异常  3 保留
		// L6 前车门状态
    // 0:关 1：开
    // L7 后车门状态
    // 0:关 1：开
    
		// 其他位保留待定，设置为0。
		DWORD dwAlertSta = g_objMonAlert.GetAlertSta();
		DWORD dwDevSta = g_objQianIO.GetDeviceSta();
		BYTE bytH = 0, bytL = 0;
		
		if( g_objSock.IsOnline() ) bytH |= 0x01; // 网络在线情况
		if( dwAlertSta & ALERT_TYPE_BUMP ) bytH |= 0x02;
		if( dwAlertSta & ALERT_TYPE_OVERTURN ) bytH |= 0x04;
		if( dwAlertSta & ALERT_TYPE_POWBROWNOUT ) bytH |= 0x08;
		if( dwAlertSta & ALERT_TYPE_POWOFF ) bytH |= 0x10;
		if( dwAlertSta & ALERT_TYPE_FOOT ) bytH |= 0x20;
		if( dwAlertSta & ALERT_TYPE_DRIVE ) bytH |= 0x40;

		bytL |= 0; // 暂时都强制使用"东经北纬"
		if( '3' == gps.m_cFixType ) bytL |= 0x08; // 定位类型
		else if( '2' == gps.m_cFixType ) bytL |= 0x04;
		else bytL |= 0;
		if( dwDevSta & DEVSTA_GPSON ) bytL |= 0; // GPS模块状态
		else bytL |= 0x10;
		if( dwAlertSta & ALERT_TYPE_FRONTDOOR ) bytL |= 0x40; // 前车门状态
		if( dwAlertSta & ALERT_TYPE_BACKDOOR ) bytL |= 0x80; // 后车门状态
		
		frm.inf[c_snd].state[0] = bytH;
		frm.inf[c_snd].state[1] = bytL;

		BYTE byt = 0;
		if( dwAlertSta & ALERT_TYPE_BUMP ) byt |= 0x01;
		if( dwAlertSta & ALERT_TYPE_OVERTURN ) byt |= 0x02;
		if( dwAlertSta & ALERT_TYPE_POWBROWNOUT ) byt |= 0x04;
		if( dwAlertSta & ALERT_TYPE_POWOFF ) byt |= 0x08;
		if( dwAlertSta & ALERT_TYPE_FOOT ) byt |= 0x10;
		if( dwAlertSta & ALERT_TYPE_BELOWSPEED ) byt |= 0x20;
		
		frm.inf[c_snd].sensor2 = byt & 0x3f;
	}

	frm.inf[c_snd].ct_load = 4;
	frm.inf[c_snd].ad_load[0] = 0;
	frm.inf[c_snd].ad_load[1] = 0;
	frm.inf[c_snd].ad_load[2] = 0;
	frm.inf[c_snd].ad_load[3] = 0;

	{
		byte bytRptType = 0;
		byte bytOilSta1 = 0;
		const double OIL_MVESPD1 = 1.852 * 2 / 3.6; // 米/秒
		bool bGpsValid1 = gps.valid == 'A' ? true : false;
		bool bAccValid1 = g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID ? true : false;
	
		byte bytOilSta = 0; 
		if( !bAccValid1 ) bytOilSta  |= 0x10;
		else if( !bGpsValid1 ) bytOilSta |= 0x04;
		else
		{
			if( gps.speed >= OIL_MVESPD1 ) bytOilSta |= 0x02;
			else bytOilSta |= 0x08;
		}  

		if(!g_objOil.bConnect()) bytOilSta |= 0x20; 	//油耗盒是否连接
	
		bool bytGetOilSucc = false;
		ushort ValidOilAD = 0,ValidPowtAD = 0;
		ushort RealOilAD = 0,RealPowAD = 0;

// 		if(bOilStatus)
		{
			bytGetOilSucc = g_objOil.GetOilAndPowAD(bytRptType, bytOilSta1, ValidOilAD, ValidPowtAD);
			
			if(bytGetOilSucc)
			{
				if(bytOilSta1&0x01) bytOilSta |= 0x01;		// 点火标志
				if(bytOilSta1&0x08) bytOilSta |= 0x80;		// 加漏油标志
				if(bytOilSta1&0x10) bytOilSta |= 0x40;		// 超出范围标志
			}
		}

		frm.inf[c_snd].m_bytOilSta = bytOilSta;
		frm.inf[c_snd].m_wValidOilAD = htons(ValidOilAD);
		frm.inf[c_snd].m_wValidPowtAD = htons(ValidPowtAD);		
	}
	
	// 达到发送周期则最后一帧时填入总里程
	if( c_snd >= (sndcyc-1) ) { 
		frm.meter = htonl(ulong(m_v.metertotal));
	}
}
#else
void CDriveRecord::fill_frm3747(frm3747 &frm, int c_snd, ushort cltcyc, ushort sndcyc, tagGPSData gps)
{
	if(0==c_snd)	//采集第一个GPS点时
	{
		frm.reply = 0;
		frm.w_size = m_cfg.winsize;
		frm.frmno = 0;
		frm.ad_oil = 0;
		frm.vo_oil = 0;
		frm.st_oil = 0;
		frm.tm_spc = htons(cltcyc);
		frm.ct_gps = byte(sndcyc);

		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);
		
		frm.date[0]	= (byte)(pCurrentTime.tm_year-100);
		frm.date[1]	= (byte)(pCurrentTime.tm_mon+1);
		frm.date[2]	= (byte)pCurrentTime.tm_mday;
		frm.time[0]	= (byte)pCurrentTime.tm_hour;
		frm.time[1]	= (byte)pCurrentTime.tm_min;
		frm.time[2] = (byte)pCurrentTime.tm_sec;
	}

	frm.inf[c_snd].numdrv = htons(ushort(m_v.numdrv));
	double db = 60*(gps.latitude-byte(gps.latitude));
	frm.inf[c_snd].lat[0] = byte(gps.latitude);
	frm.inf[c_snd].lat[1] = (byte)db;
	frm.inf[c_snd].lat[2] = byte(long(db*100)%100);
	frm.inf[c_snd].lat[3] = byte(long(db*10000)%100);
	db = 60*(gps.longitude-byte(gps.longitude));
	frm.inf[c_snd].lon[0] = byte(gps.longitude);
	frm.inf[c_snd].lon[1] = (byte)db;
	frm.inf[c_snd].lon[2] = byte(long(db*100)%100);
	frm.inf[c_snd].lon[3] = byte(long(db*10000)%100);
	frm.inf[c_snd].speed = m_v.vector[0];
	frm.inf[c_snd].dir = byte(gps.direction/3) + 1;
	
	{
		DWORD dwIoSta = 0;
		// 获取传感器信息
		g_objQianIO.IN_QueryIOSta( dwIoSta );
		// 协议定义: D7～D0：其中0表示无信号，1表示有信号
		//	D7--脚刹信号
		//  D6--手刹信号
		//  D5--左转向灯信号
		//  D4--右转向灯信号
		//  D3--前车门信号
		//  D2--后车门信号
		//  D1--电子喇叭信号
		//  D0--ACC信号
		frm.inf[c_snd].sensor = BYTE(dwIoSta % 0x100);
		
		DWORD dwVeSta = g_objCarSta.GetVeSta();
		if(dwVeSta & VESTA_HEAVY)//重车
		{
			frm.inf[c_snd].sensor |= 0x02;
		}
		else		//空车
		{
			frm.inf[c_snd].sensor &= 0xFD;
		}
		// 获取状态信息
		
		// 协议定义:
		// 状态字：H7 H6 H5 H4 H3 H2 H1 H0
		//		     L7 L6 L5 L4 L3 L2 L1 L0
		// H0：GPRS在线情况  0不在线        1在线
		// H1：发生碰撞       0 无碰撞       1 碰撞
		// H2：发生侧翻       0无侧翻       1 侧翻
		// H3：欠压状态       0 无欠压       1欠压
		// H4：主电断电状态   0 主电未断电   1主电断电
		// H5：抢劫报警状态   0无抢劫报警   1发生抢劫报警
		// H6：非法启动状态   0 无非法启动   1 发生非法启动
		// H7：保留
		// 
		// L1、L0经纬度坐标:
		// 0  东经北纬  1  东经南纬  2  西经北纬  3  西经南纬
		// L3、L2 GPS模块定位状态：
		// 0：未定位    1：2D定位    2：3D定位   3 保留
		// L5、L4 GPS模块工作状态：
		// 0：正常      1：省电       2：通讯异常  3 保留
		// L6 前车门状态
    // 0:关 1：开
    // L7 后车门状态
    // 0:关 1：开

		// 其他位保留待定，设置为0。
		DWORD dwAlertSta = g_objMonAlert.GetAlertSta();
		DWORD dwDevSta = g_objQianIO.GetDeviceSta();
		BYTE bytH = 0, bytL = 0;
		
		if( g_objSock.IsOnline() ) bytH |= 0x01; // 网络在线情况
		if( dwAlertSta & ALERT_TYPE_BUMP ) bytH |= 0x02;
		if( dwAlertSta & ALERT_TYPE_OVERTURN ) bytH |= 0x04;
		if( dwAlertSta & ALERT_TYPE_POWBROWNOUT ) bytH |= 0x08;
		if( dwAlertSta & ALERT_TYPE_POWOFF ) bytH |= 0x10;
		if( dwAlertSta & ALERT_TYPE_FOOT ) bytH |= 0x20;
		if( dwAlertSta & ALERT_TYPE_DRIVE ) bytH |= 0x40;

		bytL |= 0; // 暂时都强制使用"东经北纬"
		if( '3' == gps.m_cFixType ) bytL |= 0x08; // 定位类型
		else if( '2' == gps.m_cFixType ) bytL |= 0x04;
		else bytL |= 0;
		if( dwDevSta & DEVSTA_GPSON ) bytL |= 0; // GPS模块状态
		else bytL |= 0x10;
		if( dwAlertSta & ALERT_TYPE_FRONTDOOR ) bytL |= 0x40; // 前车门状态
		if( dwAlertSta & ALERT_TYPE_BACKDOOR ) bytL |= 0x80; // 后车门状态
		
		frm.inf[c_snd].state[0] = bytH;
		frm.inf[c_snd].state[1] = bytL;

		BYTE byt = 0;
		if( dwAlertSta & ALERT_TYPE_BUMP ) byt |= 0x01;
		if( dwAlertSta & ALERT_TYPE_OVERTURN ) byt |= 0x02;
		if( dwAlertSta & ALERT_TYPE_POWBROWNOUT ) byt |= 0x04;
		if( dwAlertSta & ALERT_TYPE_POWOFF ) byt |= 0x08;
		if( dwAlertSta & ALERT_TYPE_FOOT ) byt |= 0x10;
		if( dwAlertSta & ALERT_TYPE_BELOWSPEED ) byt |= 0x20;
		
		frm.inf[c_snd].sensor2 = byt & 0x3f;
	}

	frm.inf[c_snd].ct_load = 4;
	frm.inf[c_snd].ad_load[0] = 0;
	frm.inf[c_snd].ad_load[1] = 0;
	frm.inf[c_snd].ad_load[2] = 0;
	frm.inf[c_snd].ad_load[3] = 0;
	
	// 达到发送周期则最后一帧时填入总里程
	if( c_snd >= (sndcyc-1) )
	{ 
		frm.meter = htonl(ulong(m_v.metertotal));
	}
}
#endif

//------------------------------------------------------------------------------
// 用于主动上报行驶数据(带油耗)
void CDriveRecord::deal_updrv(tagGPSData gps)
{
	static LinkLst link;		//临时存放采集数据的链表类
	
	// ---------- 处理采集行驶数据的状态机 --------------
#if USE_OIL == 3
	static frm3749 frm = {0};
#else
	static frm3747 frm = {0};
#endif

	static int c_snd = 0;		//当前帧采集到第几个GPS点
	static byte sta_idlst = 0;	//上一次的状态标识

	// 根据车辆状态确定状态标识
	sta_idlst = m_cfg.sta_id;
	//car_move改为acc_valid，更加准确，防止调试的时候按ACC发现上报参数没有更新，从而以为车台有问题.
	if(0x01==m_cfg.sta_id || 0x02==m_cfg.sta_id)	m_cfg.sta_id = acc_valid() ? 0x01 : 0x02;
#if 0
	if(0x01==m_cfg.sta_id || 0x02==m_cfg.sta_id)	m_cfg.sta_id = car_move() ? 0x01 : 0x02;
#endif
	if(0x03==m_cfg.sta_id || 0x04==m_cfg.sta_id)	m_cfg.sta_id = !car_load() ? 0x03 : 0x04;
	if(0x05==m_cfg.sta_id || 0x06==m_cfg.sta_id)	m_cfg.sta_id = acc_valid() ? 0x05 : 0x06;

	// 如果状态标识有发生改变(或为零),则跳回空闲
	// (每次中心设置上报参数也会引起状态标识发生改变,因为调用了
	// 两次hdl_103e,使m_cfg.sta_id的值产生变化)
	if(sta_idlst!=m_cfg.sta_id)		
		m_v.st_clt = 0;
	if(0==m_cfg.sta_id)				
		m_v.st_clt = 0;

	switch(m_v.st_clt) 
	{
		case 0:	// 空闲状态
		{
			if(m_cfg.sta_id)
			{			
				c_snd = 0;
				m_v.uptime = 0;
				memset(&frm, 0, sizeof(frm));
				m_v.st_clt = 1;
			}
		}
			break;
		
		case 1:	// 采集状态
		{
			ushort cltcyc = m_cfg.cltcyc[m_cfg.sta_id-1];
			ushort sndcyc = m_cfg.sndcyc[m_cfg.sta_id-1];
			ushort uptime = m_cfg.uptime[m_cfg.sta_id-1];

			if (sndcyc>4)
			{
				PRTMSG(MSG_NOR, "deal_updrv sndcyc>4 err.\n");
				break;
			}
			
			if(0==cltcyc || 0==sndcyc) 
			{ 
				m_v.st_clt = 0; 
				break; 
			}

			if( 0==(m_v.ticks%cltcyc) || m_bNeedClt ) 
			{
#if USE_OIL == 3
				fill_frm3749(frm, c_snd, cltcyc, sndcyc, gps);
#else
				fill_frm3747(frm, c_snd, cltcyc, sndcyc, gps);
#endif
				if( ++c_snd >= sndcyc || m_bNeedClt ) 
				{
					if(m_bNeedClt)
					{
						m_bNeedClt = false;
					}
					c_snd = 0;
					//if(false==link.append((byte*)&frm, sizeof(frm)))//zzg mod 防止列表满后新的数据增加不进去
					if(link.getcount()>=1024)//zzg mod 防止列表满后新的数据增加不进去，1024，链表的最大容量
					{
						link.gohead();
						int i=0;
						for( i=0; i<m_cfg.winsize; i++)	link.next();
						for( i=0; i<m_cfg.winsize; i++)	link.remove();//清除第二个窗口，后面的窗口会自动挪进来			
					}
					link.append((byte*)&frm, sizeof(frm));
					memset(&frm, 0, sizeof(frm));
				}

				if( 0xffff!=uptime && (m_v.uptime/60)>=uptime )
				{
					PRTMSG(MSG_NOR, "Uptime(%u) is over!\n", uptime);
					m_v.st_clt = 2;
					break;
				}
			}

			if(0xffff!=uptime)
				m_v.uptime++;
		}
			break;

		case 2:	// 采集结束状态(采集总时长到)
			break;

		default:
			m_v.st_clt = 0;
			break;
	}

	// ---------- 处理上报行驶数据的状态机 --------------
	static enum 
	{ 
		ChkLink, 
		Sending, 
		WaitReply, 
		ReSending,
	} st_snd = ChkLink;			//采集以及上报行驶数据的状态机
	static bool winid = false;	//窗口标识(识别不同的窗口)
	static int overtime = 0;	//发送后等待中心应答超时计数	

#if SND_TYPE == 1// 逐帧数据发送，满一个窗口要求应答
	// 发送状态机
	static unsigned long ucSendCnt = 0;
	static unsigned long ulLastLinkCnt = 0;

	switch(st_snd) 
	{
		case ChkLink:	// 检链表内是否有数据需要发送
		{
			unsigned long tempCnt = link.getcount();

			if( ( (tempCnt >= 1) && (tempCnt != ulLastLinkCnt)  ) || tempCnt > m_cfg.winsize  )
			{
				st_snd = Sending; 

				if (0==(ucSendCnt%m_cfg.winsize) )
				{
					winid = !winid;
				}
			}
			ulLastLinkCnt = tempCnt;
		}
			break;

		case Sending:	// 发送状态
		{
			ulong tmp = 0;

#if USE_OIL == 3
			frm3749 frm = {0};
#else
			frm3747 frm = {0};
#endif
			
			link.gohead();

			int iLoop = (ucSendCnt%m_cfg.winsize) ;
			for(int kk=0 ; kk< iLoop ; kk++)
				link.next();

			link.getdata((byte*)&frm);

			PRTMSG(MSG_NOR, "snd frm3747 Tm=%02d:%02d:%02d.iLoop=%d.\n",frm.time[0],frm.time[1],frm.time[2],iLoop);
			
			frm.reply = ( (ucSendCnt+1) % m_cfg.winsize ) ? 0x42 : 0x43;
			frm.reply = winid ? (frm.reply & ~0x08) : (frm.reply | 0x08);	
			frm.frmno = byte(ucSendCnt%m_cfg.winsize );
			
			int ln = frm.mkfrm();

#if USE_OIL == 3
			tcp_send(0x37, 0x49, &frm, ln);
#else
			tcp_send(0x37, 0x47, &frm, ln);
#endif
			
			if ( 0==( (ucSendCnt+1) % m_cfg.winsize ) )
			{
				overtime = 0;
				memset(m_v.rp_oil, 0xff, 5);
				st_snd = WaitReply;
			}
			else
			{
				st_snd = ChkLink; 
			}
			ucSendCnt++;
		}
			break;

		case ReSending:	// 重发状态
		{
			ulong tmp = 0;

#if USE_OIL == 3
			frm3749 frm = {0};
#else
			frm3747 frm = {0};
#endif
			memcpy(&tmp, &m_v.rp_oil[1], 4);

			tmp = htonl(tmp);	//字节序需要转换
			tmp &= (~(0xffffffff<<m_cfg.winsize));		//将无用位清0

			link.gohead();

			for(int i=0; i<m_cfg.winsize; i++) 
			{
				if(tmp & 0x00000001) 
				{
					link.getdata((byte*)&frm);
					frm.reply = (tmp>>1) ? 0x42 : 0x43;		//(tmp>>1)为0,则表示是最后一帧
					frm.reply = winid ? (frm.reply & ~0x08) : (frm.reply | 0x08);
					frm.frmno = byte(i);
				
					int ln = frm.mkfrm();
#if USE_OIL == 3
					tcp_send(0x37, 0x49, &frm, ln);
#else
					tcp_send(0x37, 0x47, &frm, ln);
#endif
				}
				tmp >>= 1;
				link.next();
			}

			overtime = 0;
			memset(m_v.rp_oil, 0xff, 5);
			st_snd = WaitReply;
		
			PRTMSG(MSG_NOR, "ReSend(%u/%u) and wait rp_oil...\n", m_cfg.winsize, link.getcount());
		}
			break;

		case WaitReply:		// 等待中心应答状态
		{
			if(0x00 == m_v.rp_oil[0])	//该窗口中心未收全
			{		
				st_snd = ReSending; 
				PRTMSG(MSG_NOR, "Rcv rp_oil(part)!\n");
				break; 
			}
			if(0x01 == m_v.rp_oil[0])	//该窗口中心接收完毕
			{		
				link.gohead();
				for(int i=0; i<m_cfg.winsize; i++)	link.remove(); 
				ucSendCnt = ucSendCnt - m_cfg.winsize;	
				if (ucSendCnt<0) ucSendCnt=0;
				PRTMSG(MSG_NOR, "Rcv rp_oil(all)!\n");	
				st_snd = ChkLink; 		
				break; 
			}
			if(++overtime > 3*60)		//中心无应答,超时!
			{					
				if( g_objSock.IsOnline() ) //若不在线则没必要进行重发，避免占用过多的TCP队列空间// zzg mod
				{
					PRTMSG(MSG_NOR, "Wait rp_oil overtime!\n");//中心无应答,超时!
					st_snd = ChkLink; 
				}
				else
				{
					overtime=0;
				}
				break; 
			}
		}
			break;

		default:
			st_snd = ChkLink;
			break;
	}	
#endif


#if SND_TYPE == 2// 满一个窗口再发送
	// 发送状态机
	switch(st_snd) 
	{
		case ChkLink:	// 检链表内是否有数据需要发送
		{
			if(link.getcount() >= m_cfg.winsize) 
			{ 
				winid = !winid; 
				memset(m_v.rp_oil, 0xff, 5);
				st_snd = Sending; 
				break; 
			}
		}
			break;

		case Sending:	// 发送状态
		{
			ulong tmp = 0;
			frm3747 frm = {0};
			memcpy(&tmp, &m_v.rp_oil[1], 4);

			tmp = htonl(tmp);	//字节序需要转换
			tmp &= (~(0xffffffff<<m_cfg.winsize));		//将无用位清0

			link.gohead();//一直发的是最早的那个窗口的数据

			for(int i=0; i<m_cfg.winsize; i++)
			{
				if(tmp & 0x00000001) 
				{
					link.getdata((byte*)&frm);
					
					frm.reply = (tmp>>1) ? 0x42 : 0x43;		//(tmp>>1)为0,则表示是最后一帧
					frm.reply = winid ? (frm.reply & ~0x08) : (frm.reply | 0x08);
					frm.frmno = byte(i);
				
					int ln = frm.mkfrm();
					tcp_send(0x37, 0x47, &frm, ln);
				}

				tmp >>= 1;
				link.next();
			}
			overtime = 0;
			memset(m_v.rp_oil, 0xff, 5);
			st_snd = WaitReply;
		
			PRTMSG(MSG_NOR, "Send(%u/%u) and wait rp_oil...\n", m_cfg.winsize, link.getcount());
		}
			break;

		case WaitReply:		// 等待中心应答状态
		{
			if(0x00 == m_v.rp_oil[0])	//该窗口中心未收全
			{		
				st_snd = Sending; 
				PRTMSG(MSG_NOR, "Rcv rp_oil(part)!\n");
				break; 
			}
			if(0x01 == m_v.rp_oil[0])	//该窗口中心接收完毕
			{		
				link.gohead();
				for(int i=0; i<m_cfg.winsize; i++)	link.remove(); 
				PRTMSG(MSG_NOR, "Rcv rp_oil(all)!\n");
				st_snd = ChkLink; 
				break; 
			}
			if(++overtime > 3*60) 		//中心无应答,超时!
			{		
				PRTMSG(MSG_NOR, "Wait rp_oil overtime!\n");
				st_snd = ChkLink; 
				break; 
			}
		}
			break;

		default:
			st_snd = ChkLink;
			break;
	}
#endif
}


//------------------------------------------------------------------------------
// 用于处理查询数据的定时上传操作
// (每秒发送一个数据包,避免同时发送大量的数据报阻塞网络)
void CDriveRecord::deal_query()
{
	// 上传事故疑点数据
	if( 0 == m_v.ticks%4 ) 
	{
		static byte index = 0;

		if(0 != m_quedoub.getcount()) 
		{
			RcdDoub rcd = {0};
			m_quedoub.pop((byte*)&rcd);
			send_3644(&rcd, index, m_quedoub.getcount() ? 0x01 : 0x02 );
			index++;
		} 
		else 
		{ 
			index = 0; 
		}
	}

	// 上传行驶记录数据
	if( 0 == m_v.ticks%1 ) 
	{
		static byte index = 0;
		if(0!=m_quedrv.getcount()) 
		{
			RcdDrv rcd = {0};
			m_quedrv.pop((byte*)&rcd);
			frm3646 frm = {0};

			frm.reply = m_quedrv.getcount() ? 0x01 : 0x02 ;
			frm.index = index++;
			frm.count = 1;
			frm.rcd = rcd;
			frm.rcd.space = htons(frm.rcd.space);

			tcp_send(0x36, 0x46, &frm, sizeof(frm) );
		} 
		else 
		{ 
			index = 0;
		}
	}

	// 上传里程记录数据
	if( 0 == m_v.ticks%1 )
	{
		static byte index = 0;

		if(0!=m_quemeter.getcount())
		{
			RcdDrv rcd = {0};
			m_quemeter.pop((byte*)&rcd);
			frm0945 frm = {0};

			frm.reply = m_quemeter.getcount() ? 0x01 : 0x02 ;
			frm.index = index;
			frm.count = 1;
			frm.numdrv = rcd.numdrv;
			frm.bit_g0 = 0x04;
			frm.bit_g1 = rcd.s_lon[0] & 0x80;
			frm.bit_g2 = rcd.s_lon[0] & 0x40;
			rcd.s_lon[0] &= 0x3f;
			frm.bit_g3 = rcd.e_lon[0] & 0x80;
			frm.bit_g4 = rcd.e_lon[0] & 0x40;
			rcd.e_lon[0] &= 0x3f;
			memcpy(frm.s_time, rcd.s_time, 5);
			memcpy(frm.s_lon, rcd.s_lon, 4);
			memcpy(frm.e_time, rcd.e_time, 5);
			memcpy(frm.s_lat, rcd.s_lat, 4);
			frm.space[0] = byte(rcd.space/100);
			frm.space[1] = byte(rcd.space%100);

			tcp_send(0x09, 0x45, &frm, sizeof(frm) );
		}
		else
		{ 
			index = 0; 
		}
	}
}


//------------------------------------------------------------------------------
// 用于处理报警时间段和报警区域
void CDriveRecord::deal_alarm(tagGPSData gps)
{
	// 30秒扫描一次
	if(0 != (m_v.ticks%30))		return;

	// ------ 扫描报警行驶时间段 -----------------------------
	// 判断是否在规定时间段外行驶(只有在所有的时间段外行驶,才认为报警)
	if( (CAR_RUNNING==m_v.stacar) || acc_valid() )
	{
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);

		byte curtime[2] = { (byte)pCurrentTime.tm_hour, (byte)pCurrentTime.tm_min };
		
		bool outtime = true;
		int invalid = 0;

		for(int i=0; i<20; i++)
		{
			int r0 = memcmp(&m_v.almtime[i][0], &m_v.almtime[i][2], 2);
		
			if(r0>=0)	
			{ 
				invalid++;
				continue; 
			}
			
			int r1 = memcmp(curtime, &m_v.almtime[i][0], 2);
			int r2 = memcmp(curtime, &m_v.almtime[i][2], 2);
			
			if(r1>=0 && r2<=0)
			{ 
				outtime = false; 
				break;
			}
		}

		if(invalid>=20)	
			outtime = false;

		set_alarm(ALERT_TYPE_TIME, outtime);	
	} 
	else
	{
		set_alarm(ALERT_TYPE_TIME, false);
	}

	// ------- 扫描报警行驶区域 -------------------------------
	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
	if(NULL==fp)
		return;
	
	Range rg;
	int cut = 0;
	bool outrange = false;
	bool overspeed = false;
	byte dot0[8], dot1[8], dot2[8];
	
	double db = 60*(gps.longitude-byte(gps.longitude));
	dot0[0] = byte(gps.longitude);
	dot0[1] = (byte)db;
	dot0[2] = byte(long(db*100)%100);
	dot0[3] = byte(long(db*10000)%100);
	db = 60*(gps.latitude-byte(gps.latitude));
	dot0[4] = byte(gps.latitude);
	dot0[5] = (byte)db;
	dot0[6] = byte(long(db*100)%100);
	dot0[7] = byte(long(db*10000)%100);	
	
	for(int i=0; i<m_v.r_used; i++) 
	{
		fseek(fp, offsetof(RcdFile, range)+i*sizeof(Range), 0);
		fread(&rg, 1, sizeof(rg), fp);
	
		for (int j = rg.n_start;;) 
		{
			if(j >= rg.n_end)
				break;

			fseek(fp, offsetof(RcdFile, dot)+j*8, 0);
			fread(dot1, 1, 8, fp);

			if(dot1[4] & 0x80)
			{ 
				j++;
				continue;
			}
			j++;
			fread(dot2, 1, 8, fp);

			if(dot2[4] & 0x80)	//纬度分量的最高位为1，表示多边形的最后1个顶点
			{
				dot2[4] &= ~0x80; 
				j++; 
			}	

			if(cut_line(dot1, dot2, dot0))
				cut++;
		}

		outrange = false;

		if(cut % 2) {        //与区域相交的边数为奇数，说明当前点在区域内
			if((rg.attr & 0x03) == 0x00)		outrange = true;
		} else {             //与区域相交的边数为偶数，说明当前点在区域外
			if((rg.attr & 0x03) == 0x01)		outrange = true;
		}
		if(outrange)		break;
	}
	fclose(fp);
	
	if(outrange) 
	{
		if(rg.vector>0 && m_v.vector[0]>rg.vector)
			overspeed = true;
		
		if(overspeed) 
		{
			set_alarm(ALERT_TYPE_RSPEED, true);
		} 
		else 
		{
			set_alarm(ALERT_TYPE_RSPEED, false);
			set_alarm(ALERT_TYPE_RANGE, true);
		}
	} 
	else
	{
		set_alarm(ALERT_TYPE_RANGE, false);
		set_alarm(ALERT_TYPE_RSPEED, false);
	}
}

//------------------------------------------------------------------------------
// 用于处理越界报警（OF 协议类型）
void CDriveRecord::deal_area(tagGPSData gps)
{
	size_t sizMax = sizeof(m_objAreaCfg.m_aryArea) / sizeof(m_objAreaCfg.m_aryArea[0]);
	long lLon = long( gps.longitude * LONLAT_DO_TO_L );
	long lLat = long( gps.latitude * LONLAT_DO_TO_L );
	bool bInArea;
	bool bAlerm = false;

	for( BYTE i = 0; i < m_objAreaCfg.m_bytAreaCount && i < sizMax; i ++ )
	{
		bInArea = JugPtInRect(lLon, lLat, m_objAreaCfg.m_aryArea[i][0], m_objAreaCfg.m_aryArea[i][1],
			m_objAreaCfg.m_aryArea[i][2], m_objAreaCfg.m_aryArea[i][3]);
		
		switch( m_objAreaCfg.m_aryAreaProCode[ i ] & 0x03 )
		{
// 		case 0: // 区域内与
// 			{
// 				if( !bInArea )
// 				{
// 					bArea = false;
// 					goto AREA_LOOPEND;
// 				}
// 				else
// 				{
// 					bytAreaCode = (m_objAreaCfg.m_aryAreaProCode[ i ] & 0x3c) >> 2;
// 					bArea = true;
// 					goto _DEAL_AREA_END;
// 				}
// 			}
//			break;
		
		case 1: // 区域内或
			{
				if( bInArea )
				{
					m_objAreaCfg.m_aryAreaContCt[ i ] ++;
					if( POS_CONTCOUNT == m_objAreaCfg.m_aryAreaContCt[ i ] )
					{
						m_bytAreaCode = (m_objAreaCfg.m_aryAreaProCode[ i ] & 0x3c) >> 2;
						bAlerm = true;
						goto _DEAL_AREA_END;
					}
				}
				else
					m_objAreaCfg.m_aryAreaContCt[ i ]  = 0;
			}
			break;
			
// 		case 2: // 区域外与
// 			{
// 				if( bInArea )
// 				{
// 					bArea = false;
// 					goto AREA_LOOPEND;
// 				}
// 				else
// 				{
// 					bytAreaCode = (m_objAreaCfg.m_aryAreaProCode[ i ] & 0x3c) >> 2;
// 					bArea = true;
// 					goto _DEAL_AREA_END;
// 				}
// 			}
// 			break;
			
		case 3: // 区域外或
			{
				if( !bInArea )
				{
					m_objAreaCfg.m_aryAreaContCt[ i ] ++;
					if( POS_CONTCOUNT == m_objAreaCfg.m_aryAreaContCt[ i ] )
					{
						m_bytAreaCode = (m_objAreaCfg.m_aryAreaProCode[ i ] & 0x3c) >> 2;
						bAlerm = true;
						goto _DEAL_AREA_END;
					}
				}
				else
					m_objAreaCfg.m_aryAreaContCt[ i ] = 0;
			}
			break;
			
		default:
			;
		}
	}

_DEAL_AREA_END:
	if( bAlerm )
	{
		g_objMonAlert.SetAlertArea(true);

		// 报警之后清零
		for( BYTE i = 0; i < m_objAreaCfg.m_bytAreaCount && i < sizMax; i ++ )
			m_objAreaCfg.m_aryAreaContCt[ i ] = 0;
	}
}

//------------------------------------------------------------------------------
// 用于处理里程统计业务
void CDriveRecord::deal_meter(tagGPSData gps)
{
	// 每秒钟统计一下行驶里程
	double dis = 0;
	static ulong plslst = 0;
	ulong plscur = 0;

// 	plscur = g_objComBus.GetCycleCount();
// 
// 	if( plscur>plslst && 0!=m_cfg.factor )	//如果有里程脉冲信号,则采用脉冲统计里程
// 	{		
// 		dis = (plscur-plslst)/m_cfg.factor;
// 		ulong speed = 0;
// 		g_objQianIO.IN_QueryIOSpeed(speed);
// 		gps.speed = float((speed/m_cfg.factor)*1000); 
// 	}
//	else	//否则用GPS信号统计
	{	
		if( 0==m_v.lstlon || 0==m_v.lstlat || !acc_valid() )
			dis = 0;
		else
			dis = G_CalGeoDis2(m_v.lstlon, m_v.lstlat, gps.longitude, gps.latitude);

		// 乘以里程校验系数（经验值）
		if( gps.speed > 22.0 )
		{
			dis *= 1.01;
		}
		else
		{
			dis *= 1.02;
		}
	}
	
	if (dis>100) 
	{
        dis = 0;
	}

	// 针对过山洞、隧道导致GPS无效的情况，对里程统计做以下调整：
	// 1) 若首次发现GPS无效，则用之前10秒内的有效速度的平均值，作为GPS无效期内的车辆速度V；
	// 2) 从无效恢复有效时，计算无效时间，若无效时间小于10分钟，则：里程 += V * 无效时间
	// 3) 若无效时间超过10分钟，则不予处理；
	static bool bFirstInValid = true;
	static byte bytVector = 0;
	static DWORD dwInValidCount = 0;

	byte bytsta = 0;
	IOGet(IOS_ACC, &bytsta);

	if( IO_ACC_ON == bytsta )
	{
		if( 'V' == gps.valid && true == bFirstInValid )
		{
			bFirstInValid = false;
			
			// 记录首次无效时刻
			dwInValidCount = GetTickCount();
			
			// 记录前10秒的平均速度
			int iVector = 0;
			for(int k=0; k<10; k++)		iVector += m_v.vector[k];
			bytVector = (byte)( iVector * 1.852 / 3.6 / 10 );
		}
		if( 'A' == gps.valid && false == bFirstInValid )
		{
			bFirstInValid = true;
			
			// 若无效时间小于10分钟
			DWORD dwCount = GetTickCount() - dwInValidCount;
			if( dwCount < 10 * 60 * 1000 )
			{
				dis = bytVector * dwCount / 1000;
			}
		}
	}
	else
	{
		bFirstInValid = true;
		bytVector = 0;
		dwInValidCount = 0;
	}
	

	plslst = plscur;
	m_v.lstlon = gps.longitude;
	m_v.lstlat = gps.latitude;
	m_v.metercur += dis;
	m_v.metertotal += dis;
	m_v.m360._space += dis;
	m_v.m2d._space += dis;

#if USE_LIAONING_SANQI == 1
	TotalMeter = (ushort)(m_v.metertotal/1000);
	Mile = (ulong)(m_v.metertotal/1000);
#endif
	// 统计速度
	for(int i=59; i>=1; i--)	m_v.vector[i] = m_v.vector[i-1];
	m_v.vector[0] = byte( ((gps.speed*3600)/1000)/1.852 );
	m_v.speed = byte( ((gps.speed*3600)/1000) );
	
	//090531
	if ('A'!=gps.valid) 
	{
		m_v.vector[0] = 0;
		m_v.speed	 = 0;
	}
	
	//五分钟记录一次,防止车台复位导致里程统计误差过大
	if( 0 == (m_v.ticks%300) )
	{
		flush_headrcd();
	}
	// 10分钟累计一次2个日历天内的里程
	if( 0 == (m_v.ticks%600) ) 
	{
		struct tm pCurrentTime;
		G_GetTime(&pCurrentTime);

		byte date[3] = { (byte)(pCurrentTime.tm_year-100), (byte)(pCurrentTime.tm_mon+1), (byte)pCurrentTime.tm_mday };
		
		if( memcmp(date, &m_v.m2d.date[2][0], 3) >0 )
		{
			memcpy(&m_v.m2d.date[0][0], &m_v.m2d.date[1][0], 3);
			memcpy(&m_v.m2d.date[1][0], &m_v.m2d.date[2][0], 3);
			m_v.m2d.space[0] = m_v.m2d.space[1];
			m_v.m2d.space[1] = m_v.m2d.space[2];
			memcpy(&m_v.m2d.date[2][0], date, 3);
			m_v.m2d.space[2] = m_v.m2d._space;
		}
		else
		{
			m_v.m2d.space[2] += m_v.m2d._space;
		}

		m_v.m2d._space = 0;		//重置2个日历天里程计数
	}	
	
	// 2个小时记录一次360数据 
	if( 0 == (m_v.ticks%7200) )
	{
		for(int i=180-1; i>=1; i--)	
			m_v.m360.space[i] = m_v.m360.space[i-1];

		m_v.m360.space[0] = m_v.m360._space;
		m_v.m360._space = 0;	//重置2小时里程计数
		
		flush_headrcd();
		PRTMSG(MSG_NOR, "flush_headrcd : sav m360\n");
	}
}
   

//------------------------------------------------------------------------------
// 用于处理持续行驶的状态机
void CDriveRecord::deal_state()
{
	if( 0 != (m_v.ticks%60) )	return;	

//	static int prompt = 0;				//已提示次数
//	static bool alarmed = false;		//是否报警过的标志

	// 获得车辆状态(移动或静止)
	m_v.stacur = ((CAR_RUNNING==m_v.stacar) && acc_valid()) ? CAR_RUNNING : CAR_STOP;
	
	// 将两种状态分解为四种状态
	int sta_tmp = m_v.stacur;
	if(m_v.stapre != m_v.stacur) {
		m_v.stacur = (CAR_RUNNING==m_v.stacur) ? CAR_STOP_RUN : CAR_RUN_STOP;
	}
	m_v.stapre = sta_tmp;

	// 计算平均速度
	int average = 0, i=0;
	for(i=0; i<60; i++)	
		average += m_v.vector[i];
	average = average/60;

	for(i=14; i>=1; i--)
		m_v.average[i] = m_v.average[i-1];
	m_v.average[0] = average;

	if(CAR_RUN_STOP == m_v.stacur)
	{
		flush_headrcd();	//停车时存储
		PRTMSG(MSG_NOR, "flush_headrcd : sav average\n");
	}
	
	// 检查司机是否有更换
	if(m_v.numdrv != m_v.rcddrv.numdrv)
	{
		PRTMSG(MSG_NOR, "Driver switch(%u->%u)!\n", m_v.rcddrv.numdrv, m_v.numdrv);
		m_v.run = 0;
		m_v.stop = 0;
		m_v.rcddrv.numdrv = m_v.numdrv;
	}

	switch(m_v.stacur) 
	{
	case CAR_RUNNING:
		{
			m_v.run++;

			if(chk_overtired(m_v.run))
			{
				if(!m_v.alarmed)
				{
					m_v.alarmed = true;
					alarm_overtired(m_v.run);
				}

				if(m_v.prompt < MAX_PROMPT) 
				{
					m_v.prompt++;
					play_tts("司机朋友您已疲劳行驶,请注意休息!");
					PRTMSG(MSG_NOR, "Tire drv(%uh,%umin)!", m_cfg.max_tire, m_cfg.min_sleep);

					// 向调度屏也提示
					char tmpbuf[] = {"司机朋友您已疲劳行驶,请注意休息!"};
					char buf[100] = {0};
					
					buf[0] = 0x01;
					buf[1] = 0x72;
					buf[2] = 0x01;
					memcpy(buf+3, tmpbuf, sizeof(tmpbuf));
					
					DataPush(buf, sizeof(tmpbuf)+3, DEV_QIAN, DEV_DIAODU, LV2);
				}
			}
		}
		break;
		
	case CAR_STOP_RUN:
		{
			if(m_v.stacur!=m_v.stapre)
				PRTMSG(MSG_NOR, "CAR_STOP_RUN");
			
			if(m_v.run > 0) 
			{
				m_v.run += m_v.stop;
			} 
			else
			{
				m_v.rcddrv.numdrv = m_v.numdrv;
				m_v.rcddrv.set_start_gps();
				m_v.rcddrv.set_start_time();
				m_v.metercur = 0;		//清除上次里程数
			}
			m_v.stop = 0;
		}
		break;
		
	case CAR_RUN_STOP:
		{
			if(m_v.stacur!=m_v.stapre)
				PRTMSG(MSG_NOR, "CAR_RUN_STOP\n");
			m_v.stop++;
		}
		break;
		
	case CAR_STOP:
		{
			if(m_v.run > 0) 
			{
				m_v.stop++;
				if(m_v.stop >= m_cfg.min_sleep) 
				{
					if( (m_v.run/60) >= CONTINUE_TIME )	
						flush_drvrcd();	//写入1条行驶记录	
					
					m_v.run = 0;
					m_v.stop = 0;
					m_v.prompt = 0;
					m_v.alarmed = false;
				}
			}
			else 
			{
				m_v.stop = 0;
			}
		}	
		break;
		
	default:
		m_v.stacur = CAR_STOP;
		break;	
	}
}

//------------------------------------------------------------------------------
// 用于处理司机登陆业务
void CDriveRecord::deal_login()
{
	static int sta_iLstSta = -1;
	if( sta_iLstSta != m_v.st_log )
	{
		PRTMSG(MSG_NOR, "Driver Sta: %d\n", m_v.st_log );
		sta_iLstSta = m_v.st_log;
	}

	switch(m_v.st_log) 
	{
	case UnLog:
		{
			m_v.numdrv = 0;
			m_v.inputflag = -2;
			
			if(acc_valid()) 
			{
				if(0==m_cfg.total) 
				{
					m_v.st_log = Logined;
					PRTMSG(MSG_NOR, "Driver login succ(Guest)!\n");
				} 
				else 
				{
					req_login();
					m_v.st_log = Inputing;
					PRTMSG(MSG_NOR, "Please input drvcode!\n");
					play_tts("司机朋友,请输入您的身份代码!");
				}
			}
		}
		break;
		
	case Inputing:
		{
			static int cnt = 0;
			if(cnt++>120 || (-1==m_v.inputflag) ) 
			{
				char buf[16];
				cnt = 0;
				m_v.st_log = UnLog;

				PRTMSG(MSG_NOR, "Driver login fail!\n");
				g_objMonAlert.SetAlertDrive(true);
				play_tts("司机朋友,您的输入有误,请不要非法驾驶!");

				memset(buf,0,sizeof(buf));
				buf[0] = (char)0x01;
				buf[1] = (char)0xf8;
				DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
			} 
			else if(m_v.inputflag>=0) 
			{
				cnt = 0; 
				m_v.st_log = Logined; 
				m_v.numdrv = m_v.inputflag;

				PRTMSG(MSG_NOR, "Diaodu: Login succ\n");
				
				g_objMonAlert.SetAlertDrive(false);
				flush_headrcd(); // 立即保存司机登陆状态
			
				play_tts("司机朋友,验证通过,祝您旅途愉快!");
			} 
			else if(!acc_valid()) 
			{
				m_v.numdrv = 0;
				m_v.st_log = UnLog;

				req_unlog();
				g_objMonAlert.SetAlertDrive(false);
				set_alarm(ALERT_TYPE_DRIVE, false);
			}
		}
		break;	
					
	case Logined:
		{
			if(!acc_valid())
			{
				if( m_v.numdrv != 0)
					play_tts("司机朋友,您已退出登陆！");

				m_v.numdrv = 0;
				m_v.st_log = UnLog;
				
				req_unlog();
				flush_headrcd(); // 立即保存司机未登陆状态
				
				PRTMSG(MSG_NOR, "Driver Logout!\n");
			}
		}
		break;

	default:
		break;
	}
}


//------------------------------------------------------------------------------
// 检查是否有超速
// speed: 速度,单位海里/时
bool CDriveRecord::chk_overspeed()
{
	static byte tm_over = 0;
	static bool prompt = false;
	byte speed = m_v.vector[0];	

	if( 0 == m_cfg.max_speed )
	{ 
		tm_over = 0; 
		prompt = false; 
		return false;
	}
	
	if( speed > m_cfg.max_speed ) 
	{
		if(tm_over<255)	
			tm_over++;
	}
	else 
	{
		tm_over = 0;
	}

	if( tm_over > m_cfg.min_over )
	{
		if(!prompt)
		{
			PRTMSG(MSG_NOR, "Overspeed(%usm/h,%us)!!\n", m_cfg.max_speed, m_cfg.min_over);
			play_tts("你已超速,请减速慢行!");

			g_objMonAlert.SetAlertOverSpd(true);
		}
		prompt = true;
		return true;
	}
	else
	{
		prompt = false;
		return false;
	}
}


//------------------------------------------------------------------------------
// 检查是否有低速
// speed: 速度,单位公里/时
bool CDriveRecord::chk_belowspeed()
{
	static int tm_below = 0;
	byte speed = byte(m_v.vector[0]*1.852);	
	
	if( 255 == m_cfg.min_speed )
	{
		tm_below = 0; 
		return false;
	}
	//速度为零时不进行低速报警
	if( 0 == speed )
	{
		tm_below = 0; 
		return false;
	}
	
	if( speed < m_cfg.min_speed ) 
	{
		tm_below++;
	}
	else 
	{
		tm_below = 0;
	}

	if( tm_below > m_cfg.min_below*60 )
	{
		PRTMSG(MSG_NOR, "Belowspeed(%ukm/h,%um)!!\n", m_cfg.min_speed, m_cfg.min_below);
		g_objMonAlert.SetAlertBelowSpd(true);

		tm_below = 0;
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------
// 向调度屏发送司机登录请求
void CDriveRecord::req_login()
{
	int i = 0;
	char buf[1024] = {0};
	
	buf[i++] = 0x01;	// 使用串口扩展盒，0x01表示要发送到调度屏的数据
	buf[i++] = 0x62;	//请求司机登录
	buf[i++] = m_cfg.total;
	for(byte j=0; j<10; j++)
	{
		if(m_cfg.inf[j].valid)
		{
			buf[i++] = j;
			buf[i++] = m_cfg.inf[j].num/0xff;
			buf[i++] = m_cfg.inf[j].num%0xff;
			
			byte len = min(strlen(m_cfg.inf[j].ascii), 7);
			buf[i++] = len;
			memcpy(buf+i, m_cfg.inf[j].ascii, len);
			i += len;
		}
	}

	DataPush(buf, i, DEV_QIAN, DEV_DIAODU, LV2);
}


//------------------------------------------------------------------------------
// 向调度屏发送司机取消登录指示
void CDriveRecord::req_unlog()
{
	char buf[2] = {0x01, 0x6c};//司机取消登录指示
	DataPush(buf, 2, DEV_QIAN, DEV_DIAODU, LV2);
}


//------------------------------------------------------------------------------
// 得到司机索引号
int CDriveRecord::get_drvindex(int numdrv)
{
	if(0==numdrv)
		return -1;

	for(int i=0; i<MAX_DRIVER; i++) 
	{
		if(m_cfg.inf[i].valid)
		{
			if(m_cfg.inf[i].num == numdrv)
				return i;
		}
	}

	return -1;
}

//------------------------------------------------------------------------------
// 检查事故疑点记录是否符合查询标准
bool CDriveRecord::chk_doubrcd(RcdDoub *rcd, byte timecnt, byte time[10][10], byte regioncnt, byte region[5][16])
{
	int i=0;

	// 检查停车时间是否在查询时间段内
	for(i=0; i<timecnt; i++)
	{
		int r1 = memcmp(rcd->times, &time[i][0], 5);
		int r2 = memcmp(rcd->times, &time[i][5], 5);
	
		if(r1>=0 && r2<=0)
			break;

		if((timecnt-1)==i)
			return false;
	}
	
	// 检查位置是否在查询的位置范围内
	for(i=0; i<regioncnt; i++) 
	{
		int r1 = memcmp(rcd->pos[0].lon, &region[i][0], 4);
		int r2 = memcmp(rcd->pos[0].lat, &region[i][4], 4);
		int r3 = memcmp(rcd->pos[0].lon, &region[i][8], 4);
		int r4 = memcmp(rcd->pos[0].lat, &region[i][12], 4);
		
		if(r1>=0 && r2>=0 && r3<=0 && r4<=0)
			break;

		if((regioncnt-1)==i)
			return false;
	}
	
	return true;
}


//------------------------------------------------------------------------------
// 检查行驶记录是否符合查询标准(检查每条记录的行驶时间是否大于查询时间值)
bool CDriveRecord::chk_drvrcd(RcdDrv *rcd, byte numdrv, byte times)
{
	struct tm pCurrentTime;
	G_GetTime(&pCurrentTime);
	
	// 检查是否为2个日历天内的记录
	int interval = count_days((pCurrentTime.tm_year+1900), pCurrentTime.tm_mon+1, pCurrentTime.tm_mday) 
			- count_days(rcd->s_time[0]+2000, rcd->s_time[1], rcd->s_time[2]);
	if(interval >= 2)
		return false;
	
	// 检查司机编号是否一致(numdrv为0则不进行验证)
	if( (rcd->numdrv!=numdrv) && (numdrv!=0) )
		return false;
	
	// 检查行驶时间是否超过查询的时间
	interval = get_minutes(rcd->s_time[0]+2000, rcd->s_time[1], rcd->s_time[2], rcd->s_time[3], rcd->s_time[4],
		rcd->e_time[0]+2000, rcd->e_time[1], rcd->e_time[2], rcd->e_time[3], rcd->e_time[4]);
	if(times*60 > interval )
		return false;
	
	return true;
}


//------------------------------------------------------------------------------
// 检查行驶记录是否符合里程查询标准
bool CDriveRecord::chk_drvrcd2(RcdDrv *rcd, byte type, byte *s_time, byte *e_time, byte numdrv)
{
	// 检查司机编号是否一致(numdrv为0则不进行验证)
	if( (rcd->numdrv!=numdrv) && (numdrv!=0) )
		return false;

	switch(type) 
	{
	case 0x01:	//按照上车时间查询
		{
			int r1 = memcmp(rcd->s_time, s_time, 5);
			int r2 = memcmp(rcd->s_time, e_time, 5);
		
			if(r1>=0 && r2<=0)
				return true;
		}
		break;

	case 0x02:	//按照下车时间查询
		{
			int r1 = memcmp(rcd->e_time, s_time, 5);
			int r2 = memcmp(rcd->e_time, e_time, 5);
			
			if(r1>=0 && r2<=0)
				return true;
		}
		break;

	case 0x03:	//同时满足上车时间和下车时间
		{
			int r1 = memcmp(rcd->e_time, s_time, 5);
			int r2 = memcmp(rcd->e_time, e_time, 5);
			int r3 = memcmp(rcd->e_time, s_time, 5);
			int r4 = memcmp(rcd->e_time, e_time, 5);

			if( (r1>=0 && r2<=0) && (r3>=0 && r4<=0) )
				return true;
		}
		break;

	case 0x04:	//满足上车时间或下车时间
		{
			int r1 = memcmp(rcd->e_time, s_time, 5);
			int r2 = memcmp(rcd->e_time, e_time, 5);
			int r3 = memcmp(rcd->e_time, s_time, 5);
			int r4 = memcmp(rcd->e_time, e_time, 5);

			if( (r1>=0 && r2<=0) || (r3>=0 && r4<=0) )
				return true;
		}
		break;

	default:	
		break;
	}
	
	return false;
}

//------------------------------------------------------------------------------
// 发送 查询事故疑点数据应答
void CDriveRecord::send_3644(RcdDoub *rcd, byte index, byte reply)
{
	int j=0;

	// 发送携带传感器状态的数据帧
	frm36441 frm1 = {0};

	frm1.reply = 0x01;
	frm1.index = index;
	frm1.attr = 0x00;
	frm1.numdrv = rcd->numdrv;
	memcpy(frm1.times, rcd->times, 6);

	for(j=0; j<50; j++)
	{
		frm1.vecsen[j][0] = rcd->vector[j];
		frm1.vecsen[j][1] = rcd->sensor[j];
	}

	tcp_send(0x36, 0x44, &frm1, sizeof(frm1) );
		
	memset(&frm1, 0, sizeof(frm1));
	frm1.reply = 0x01;
	frm1.index = index;
	frm1.attr = 0x01;
	frm1.numdrv = rcd->numdrv;
	memcpy(frm1.times, rcd->times, 6);

	for(j=0; j<50; j++) 
	{
		frm1.vecsen[j][0] = rcd->vector[j+50];
		frm1.vecsen[j][1] = rcd->sensor[j+50];
	}

	tcp_send(0x36, 0x44, &frm1, sizeof(frm1) );
		
	// 发送携带传位置信息的数据帧
	frm36442 frm2 = {0};
	frm2.reply = 0x01;
	frm2.index = index;
	frm2.attr = 0x02;
	frm2.numdrv = rcd->numdrv;
	memcpy(frm2.times, rcd->times, 6);
	
	for(j=0; j<10; j++) 
	{
		memcpy(&frm2.gps[j][0], &rcd->pos[j], 8);
	}

	tcp_send(0x36, 0x44, &frm2, sizeof(frm2) );
		
	memset(&frm2, 0, sizeof(frm2));
	frm2.reply = reply;
	frm2.index = index;
	frm2.attr = 0x03;
	frm2.numdrv = rcd->numdrv;
	memcpy(frm2.times, rcd->times, 6);
	
	for(j=0; j<5; j++)
	{
		memcpy(&frm2.gps[j][0], &rcd->pos[j+10], 8);
	}

	tcp_send(0x36, 0x44, &frm2, sizeof(frm2) );	
}


//------------------------------------------------------------------------------
// 将文件头结构写入文件当中
bool CDriveRecord::flush_headrcd()
{
	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
	if(NULL==fp)
		return false;
	
	fseek(fp, offsetof(RcdFile,head), 0);
	fwrite(&m_v, 1, sizeof(m_v), fp);
	
	fclose(fp);
	
	return true;
}


//------------------------------------------------------------------------------
// 写入一条行驶记录 
bool CDriveRecord::flush_drvrcd()
{
	m_v.rcddrv.set_end_gps();
	m_v.rcddrv.set_end_time();
	m_v.rcddrv.space = ushort(m_v.metercur/1000);
	m_v.metercur = 0;	//清除当次里程数
	
	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");

	if(fp==NULL) 
		return false;

	fseek(fp, offsetof(RcdFile,drv)+sizeof(RcdDrv)*m_v.drvpw, 0);
	fwrite(&m_v.rcddrv, sizeof(m_v.rcddrv), 1, fp);
	
	if(++m_v.drvcnt >200 )
		m_v.drvcnt = 200;
	if(++m_v.drvpw >=200 )
		m_v.drvpw = 0;

	fseek(fp, offsetof(RcdFile,head), 0);
	fwrite(&m_v, sizeof(m_v), 1, fp);
	fclose(fp);

	PRTMSG(MSG_NOR, "flush_drvrcd, pw=%u, cnt=%u\n", m_v.drvpw, m_v.drvcnt);

	return true;
}


//------------------------------------------------------------------------------
// 将一条事故疑点记录写入文件
bool CDriveRecord::flush_doubrcd()
{	
	m_v.rcddoub.set_time();
	m_v.rcddoub.numdrv = m_v.numdrv;
	
	FILE *fp = fopen(DRIVE_RECORD_FILENAME, "rb+");
	if(NULL==fp)
		return false;

	fseek(fp, offsetof(RcdFile,doub)+sizeof(RcdDoub)*m_v.doubpw, 0);
	fwrite(&m_v.rcddoub, sizeof(m_v.rcddoub), 1, fp);
	
	if(++m_v.doubcnt >100 )
		m_v.doubcnt = 100;

	if(++m_v.doubpw >=100 )
		m_v.doubpw = 0;

	fseek(fp, offsetof(RcdFile,head), 0);
	fwrite(&m_v, sizeof(m_v), 1, fp);
	fclose(fp);
	
	PRTMSG(MSG_NOR, "flush_doubrcd, pw=%u, cnt=%u\n", m_v.doubpw, m_v.doubcnt);

	return true;
}


//------------------------------------------------------------------------------
// 根据速度设置车台的行驶状态
// vector: 速度,单位海里/时
void CDriveRecord::set_car_state(tagGPSData gps)
{
	// 根据速度确定车台状态(移动或静止)
	static int cnt = 0;
	if(CAR_STOP == m_v.stacar)
	{
		cnt = (VT_LOW_RUN>=m_v.vector[0]) ? 0 : ++cnt ;
		
		if(cnt > RUN_MINTIME) 
		{
			PRTMSG(MSG_NOR, "Car is moving(%f)!\n", m_v.metertotal);
			m_v.stacar = CAR_RUNNING;
			cnt = 0;
		}
	}
	else if(CAR_RUNNING == m_v.stacar)
	{
		cnt = (VT_LOW_RUN>=m_v.vector[0]) ? ++cnt : 0 ;
		
		if(cnt > STOP_MINTIME) 
		{
			PRTMSG(MSG_NOR, "Car is silent(%f)!\n", m_v.metertotal);
			m_v.stacar = CAR_STOP;
			cnt = 0;
		}
	} 
	else 
	{
		m_v.stacar = CAR_STOP;
	}
}


//------------------------------------------------------------------------------
// 检测持续行驶时间是否超过了设定的疲劳驾驶时间
bool CDriveRecord::chk_overtired(int minutes)
{
	if(0 == m_cfg.max_tire)	
		return false;
	if((minutes/60) >= m_cfg.max_tire)
		return true;
	else
		return false;
}


//------------------------------------------------------------------------------
// 疲劳行驶指示
// 司机编号(1)+起始时间(5)+起点经度(4)+起点纬度(4)+已行驶时间(1)
// 起始时间：格式为年、月、日、时、分
// 已行驶时间：单位为小时
bool CDriveRecord::alarm_overtired(int minutes)
{
	frm3647 frm;

	frm.numdrv = byte(m_v.numdrv);
	memcpy(frm.s_time, m_v.rcddrv.s_time, 5);
	memcpy(frm.s_lon, m_v.rcddrv.s_lon, 4);
	memcpy(frm.s_lat, m_v.rcddrv.s_lat, 4);
	frm.timed = byte(minutes/60);

	tcp_send(0x36, 0x47, &frm, sizeof(frm) );
	
	PRTMSG(MSG_NOR, "Snd tire to center(%dmin)!\n", minutes);
	return true;
}



//------------------------------------------------------------------------------	
// 计算两个时间之间的分钟数	
int CDriveRecord::get_minutes(int year, int month, int day, int hour, int minute, 
				int year2, int month2, int day2, int hour2, int minute2 )
{
	int cnt1 = count_days(year, month, day);
	int cnt2 = count_days(year2 , month2, day2);
	
	int cnt3 = 24 * 60 - (hour * 60 + minute);
	int cnt4 = hour2 * 60 + minute2;
	
	int cnt5 = (cnt3 + cnt4) + (cnt2 - cnt1 - 1) * 24 * 60 ;
	
	return cnt5;
}


//------------------------------------------------------------------------------
// 往中心发送TCP数据
void CDriveRecord::tcp_send(byte trantype, byte datatype, void *data, int len)
{
#if 0
	char str[100] = {0};
	sprintf(str, "\ntcp_send(%02x,%02x)", trantype, datatype);
	prtbuf(str, (byte*)data, len);
#endif

	int ln = 0;
	byte buf[2048] = {0};

	// 直接拷贝至buf
	if(0x01==trantype || 0x0f == trantype)
	{
		memcpy(buf, data, len);	
		ln = len;
	}

	// 里程统计协议需要将0转化为0x7f 
	if(0x09==trantype)
	{
		memcpy(buf, data, len);
		for(int i=0; i<len; i++)
		{
			if(0 == buf[i])	
				buf[i] = 0x7f;
		}

		ln = len;
	}
	
	// 行车记录仪 以及 报警设置 需要进行8转7
	if(0x36==trantype || 0x37==trantype)
	{
		ln = Code8ToCodeX(buf, (byte*)data, len, 7);
	}

	int ln2 = 0;
	char buf2[2048] = {0};
	int iRet = g_objSms.MakeSmsFrame((char*)buf, ln, trantype, datatype, buf2, sizeof(buf2), ln2);
	if( !iRet ) g_objSock.SOCKSNDSMSDATA( buf2, ln2, LV9 );
}



//------------------------------------------------------------------------------
// 计算点是否在区域内
bool CDriveRecord::cut_line(byte *dot1, byte *dot2, byte *dot0)
{
#define DegreeToNum(p)		( ((p)[0]*60 + (p)[1]) * 100 + (p)[2] )

	ulong  x1, y1, x2, y2, x0, y0, temp;
    bool positive;

    x1 = DegreeToNum(dot1);
    y1 = DegreeToNum(dot1+4);
    x2 = DegreeToNum(dot2);
    y2 = DegreeToNum(dot2+4);
    x0 = DegreeToNum(dot0);
    y0 = DegreeToNum(dot0+4);

    if (y0 == y1)	// 考虑射线经过多边形顶点、射线和多边形边平行的情况 
	{                 
        y0++;
    }
    if (y0 == y2)
	{
        y0++;
    }
    if ((y0 > y1 && y0 > y2) || (y0 < y1 && y0 < y2))
		return false;

    if (x0 < x1 && x0 < x2)
		return false;

    if (x0 > x1 && x0 > x2)
		return true;

    if (y1 > y2) 
	{
        temp = y1;
        y1   = y2;
        y2   = temp;
        temp = x1;
        x1   = x2;
        x2   = temp;
    }

    if (x2 >= x1) /* 如果：(y2 - y1) / (x2 - x1) >= (y0 - y1) / (x0 - x1)，则说明2线段相交 */
	{                 
        x2 -= x1;
        x1  = x0 - x1;
        positive = true;	/* 多边形边长的斜率为正 */
    } 
	else
	{
        x2  = x1 - x2;
        x1  = x1 - x0;
        positive = false;	/* 多边形边长的斜率为负 */
    }
    y2 -= y1;
    y1  = y0 - y1;

    if (y2 * x1 >= y1 * x2)
	{
        if (positive) 
		{
            return true;
        }
		else 
		{
            return false;
        }
    }
	else
	{
        if (positive)
		{
            return false;
        } 
		else
		{
            return true;
        }
    }
}

bool CDriveRecord::ChkDriverLog(char *v_pStrDrvNum)
{
	if (!v_pStrDrvNum)
		return 0;

	int i;
	for (i = 0;i<MAX_DRIVER;i++) 
	{
		if (!strncmp(v_pStrDrvNum, m_cfg.inf[i].ascii,min(7,strlen(v_pStrDrvNum)))) 
		{
			m_v.numdrv = m_cfg.inf[i].num;
			return true;
		}
	}

	m_v.numdrv = -1;
	return false;
}

//------------------------------------------------------------------------------
// 发送或清除报警
void CDriveRecord::set_alarm(ulong type, bool set)
{
	if(ALERT_TYPE_DRIVE==type) 
	{
		static bool seted = false;

		if(!seted && set) 
		{ 
			seted = true; 
			g_objMonAlert.SetAlertOther(type, set); 
		
			PRTMSG(MSG_NOR, "Snd Acc alarm!\n"); 
		}

		if(seted && !set) 
		{ 
			seted = false; 
			g_objMonAlert.SetAlertOther(type, set); 
		
			PRTMSG(MSG_NOR, "Cancel Acc alarm!\n"); 
		}	
	}

	if(ALERT_TYPE_TIME==type) 
	{
		static bool seted = false;

		if(!seted && set)
		{ 
			seted = true; 
			g_objMonAlert.SetAlertOther(type, set); 
		
			PRTMSG(MSG_NOR, "Snd Time alarm!!\n"); 
		}
		if(seted && !set)
		{ 
			seted = false; 
			g_objMonAlert.SetAlertOther(type, set); 
		
			PRTMSG(MSG_NOR, "Cancel Time alarm!\n"); 
		}	
	}
	
	if(ALERT_TYPE_RANGE==type) 
	{
		static bool seted = false;

		if(!seted && set) 
		{ 
			seted = true; 
			g_objMonAlert.SetAlertOther(type, set); 
			play_tts("司机朋友您已越界!");
			show_diaodu("司机朋友您已越界");
		
			PRTMSG(MSG_NOR, "Snd Area alarm!!\n"); 
		}
		if(seted && !set) 
		{ 
			seted = false; 
			g_objMonAlert.SetAlertOther(type, set); 
		
			PRTMSG(MSG_NOR, "Cancel Area alarm!"); 
		}	
	}
	
	if(ALERT_TYPE_RSPEED==type) 
	{
		static bool seted = false;
		if(!seted && set)
		{ 
			seted = true; 
			g_objMonAlert.SetAlertOther(type, set); 
			play_tts("司机朋友您已越界并超速,请减速!");
			show_diaodu("司机朋友您已越界并超速,请减速!");
		
			PRTMSG(MSG_NOR, "Snd AreaSpeed alarm!!\n"); 
		}
		if(seted && !set) 
		{ 
			seted = false; 
			g_objMonAlert.SetAlertOther(type, set); 

			PRTMSG(MSG_NOR, "Cancel AreaSpeed alarm!\n"); 
		}	
	}
}


//------------------------------------------------------------------------------
// 查询ACC状态
bool CDriveRecord::acc_valid()
{
	byte bytSta;
	IOGet(IOS_ACC, &bytSta);

	return ( bytSta == IO_ACC_ON ? true : false );
}


//------------------------------------------------------------------------------
// 查询空重车状态
bool CDriveRecord::car_load()
{
	return true;
}


//------------------------------------------------------------------------------
// 查询车辆静止移动状态
bool CDriveRecord::car_move()
{
	return (m_v.stacar==CAR_RUNNING);
}


//------------------------------------------------------------------------------
// 查询开关信号的状态
byte CDriveRecord::get_sensor()
{
// 	开关信号：D7 D6 D5 D4 D3 D2 D1 D0
// 	其中D7～D0：0表示无信号，1表示有信号
// 		D7--脚刹信号
// 		D6--手刹信号
// 		D5--左转向灯信号
// 		D4--右转向灯信号
// 		D3--前车门信号
// 		D2--后车门信号
// 		D1--电子喇叭信号
// 		D0--保留
	
// 	ulong sensor = 0;
// 	g_objQianIO.IN_QueryIOSta(sensor);
// 	return ~byte(sensor);
	
	byte sensor = 0, bytIOSignal = 0;
#if USE_COMBUS == 2	
	sensor = g_objCom150TR.GetIOSignal();
#endif
	if( sensor & 0x10 ) bytIOSignal |= 0x80;	// 脚刹
	if( sensor & 0x08 ) bytIOSignal |= 0x40;	// 手刹
	if( sensor & 0x04 ) bytIOSignal |= 0x20;	// 左灯
	if( sensor & 0x02 ) bytIOSignal |= 0x10;	// 右灯
	if( sensor & 0x01 ) bytIOSignal |= 0x08;	// 前门
	if( sensor & 0x80 ) bytIOSignal |= 0x04;	// 后门
	// 电子喇叭信号（暂空）
	
	return bytIOSignal;
}


//------------------------------------------------------------------------------
// 在调度屏上显示提示信息
// fmt,..: 格式化输入
void CDriveRecord::show_diaodu(char *v_szbuf)
{
	char str[1024] = {0x01, 0x72, 0x01};
	memcpy(str+3, v_szbuf, strlen(v_szbuf));
	DataPush(str, strlen(v_szbuf)+3, DEV_QIAN, DEV_DIAODU, LV2);
}


//------------------------------------------------------------------------------
// TTS语音播放接口
void CDriveRecord::play_tts(char *v_szbuf)
{
	char str[1024] = {0x01, 0x00};
	memcpy(str+2, v_szbuf, strlen(v_szbuf));
	DataPush(str, strlen(v_szbuf)+2, DEV_QIAN, DEV_DVR, LV2);
}

unsigned long CDriveRecord::GetTotleMeter()
{
	return (unsigned long)m_v.metertotal;
}

#endif



