// DriverCtrl.cpp: implementation of the CDriverCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "yx_QianStdAfx.h"
#include "yx_DriverCtrl.h"

#if USE_DRIVERCTRL == 1

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

const UINT DRIVERLOG_INTERVAL		= 120000; // 等待司机登陆的超时时间,ms
const UINT ACCOFF_DRVRRELOG_INTERVAL= 20000; // ACC持续无效多长时间后,需要重新进行司机登陆,ms

void G_TmDriverLog(void *arg, int len)
{
	g_objDriverCtrl.P_TmDriverLog();
}

void G_TmAccOffDriverReLog(void *arg, int len)
{
	g_objDriverCtrl.P_TmAccOffDriverReLog();
}

void G_TmFstDriverLog(void *arg, int len)
{
	g_objDriverCtrl.P_TmFstDriverLog();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDriverCtrl::CDriverCtrl()
{

}

CDriverCtrl::~CDriverCtrl()
{

}


void CDriverCtrl::P_TmDriverLog()
{
	_KillTimer(&g_objTimerMng, DRIVERLOG_TIMER );
	
	g_objMonAlert.SetAlertDrive(true);
	
	g_objCarSta.SavDriverNo( 0xffff );
	g_objCarSta.SavDrvRecSta( false );

	char buf[100] = { 0 };
	buf[0] = 0x01;
	buf[1] = 0x00;
	strcat( buf, "司机朋友,您的输入超时,请不要非法驾驶!" );
	DataPush( buf, strlen(buf), DEV_QIAN, DEV_DVR, 2 );
	
	//hxd 告诉手柄超时了
	memset(buf,0,sizeof(buf));
	buf[0] = char(0x01);
	buf[1] = char(0xf8);
	DataPush( buf, 2, DEV_QIAN, DEV_DIAODU, 2 );
}

void CDriverCtrl::P_TmAccOffDriverReLog()
{
	_KillTimer(&g_objTimerMng, ACCOFF_DRVRRELOG_TIMER );

	g_objMonAlert.SetAlertDrive(false);

	g_objCarSta.SavDriverNo( 0 );
	g_objCarSta.SavDrvRecSta( false );
}

void CDriverCtrl::P_TmFstDriverLog()
{
	// 司机登陆判断处理
	_KillTimer(&g_objTimerMng, FSTDRIVERLOG_TIMER );
	DriverCtrl( (g_objQianIO.GetDeviceSta() & DEVSTA_ACCVALID) ? true : false );
}

void CDriverCtrl::Init()
{
	// 开启初始登陆定时器 (要等调度屏也启动了)
	_SetTimer(&g_objTimerMng, FSTDRIVERLOG_TIMER, 5000, G_TmFstDriverLog );
}

void CDriverCtrl::Release()
{
}

// 司机登陆控制
void CDriverCtrl::DriverCtrl( bool v_bAccOn )
{
	_KillTimer(&g_objTimerMng, FSTDRIVERLOG_TIMER ); // 防止ACC变化已提示司机登陆后,还进行首次登陆

	static BYTE sta_bytSta = 0; // 0,初始态; 1,ACC ON; 2,ACC OFF

	if( v_bAccOn )
	{
		// 去抖
		if( 1 == sta_bytSta ) return;
		sta_bytSta = 1;

		// 关闭ACC持续无效使得司机需要重新登陆的定时
		_KillTimer(&g_objTimerMng, ACCOFF_DRVRRELOG_TIMER );

		WORD wCurDriverNo = g_objCarSta.GetDriverNo();
		if( 0 == wCurDriverNo || 0xffff == wCurDriverNo ) // 若当前没有有效司机信息
		{
			// 读取配置
			tag2QDriverCfg objDrvrCfg;
			objDrvrCfg.Init();
			::GetSecCfg( &objDrvrCfg, sizeof(objDrvrCfg), offsetof(tagSecondCfg, m_uni2QDriverCfg.m_obj2QDriverCfg),
				sizeof(objDrvrCfg) );

			// 填充请求司机登陆帧: 0x62 + 有效编号个数（1）＋【序号（1）＋编号（2）＋身份代码长度（1）＋身份代码】*N
			char buf[ 500 ];
			memset( buf, 0, sizeof(buf) );
			buf[0] = 0x01;		// Qian发往调度的，前面都要加0x01
			buf[1] = 0x62;
			DWORD dw = 3;
			WORD wTemp;
			bool bValidDriverCfgExist = false; // 是否配置了有效的司机编号
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
			tag2QDriverPar_QIAN* aryDrvrPar = objDrvrCfg.m_aryDriver;
#endif
#if USE_PROTOCOL == 30
			tag2QDriverPar_KE* aryDrvrPar = objDrvrCfg.m_aryDriver;
#endif
			const size_t DRIVER_MAXCOUNT = sizeof(objDrvrCfg.m_aryDriver) / sizeof(objDrvrCfg.m_aryDriver[0]);
			for( size_t siz = 0; siz < DRIVER_MAXCOUNT; siz ++ )
			{
				// 检查该记录是否存在有效司机信息
				if( aryDrvrPar[siz].m_wDriverNo < 1 ) continue;
				else bValidDriverCfgExist = true;

				// 存储空间检查
				if( sizeof(buf) < dw + 4 + sizeof(aryDrvrPar[0].m_szIdentID) ) break;

				// 填充发送到调度屏的帧
				buf[ dw ] = BYTE(siz); // 序号
				wTemp = htons( aryDrvrPar[siz].m_wDriverNo );
				memcpy( buf + dw + 1, &wTemp, 2); // 编号
				if( 0 == aryDrvrPar[siz].m_szIdentID[ sizeof(aryDrvrPar[0].m_szIdentID) - 1] )
					wTemp = strlen(aryDrvrPar[siz].m_szIdentID);
				else
					wTemp = sizeof(aryDrvrPar[siz].m_szIdentID);
				if( wTemp > sizeof(aryDrvrPar[siz].m_szIdentID) ) wTemp = sizeof(aryDrvrPar[siz].m_szIdentID);
				buf[ dw + 3 ] = BYTE(wTemp); // 身份代码长度
				memcpy( buf + dw + 4, aryDrvrPar[siz].m_szIdentID, wTemp ); // 身份代码

				// 有效编号个数
				buf[2] = BYTE(buf[2]) + 1;

				dw += 4 + wTemp;
			}

			// 若配置中不存在有效的司机信息,则不需要司机登陆
			if( !bValidDriverCfgExist ) return;

			// 发送请求司机登陆帧到调度屏
			DataPush(  buf, dw, DEV_QIAN,  DEV_DIAODU, 3 );

			// 开启登陆超时定时器
			_KillTimer(&g_objTimerMng, DRIVERLOG_TIMER );
			_SetTimer(&g_objTimerMng, DRIVERLOG_TIMER, DRIVERLOG_INTERVAL, G_TmDriverLog );

			// 播放请求司机登陆的提示
			buf[0] = 0x01;
			buf[1] = 0x00;
			strcat( buf, "司机朋友,请输入您的身份代码!" );
			DataPush(  buf, strlen(buf), DEV_QIAN,  DEV_DVR, 2 );
		}
	}
	else
	{
		// 去抖
		if( 2 == sta_bytSta ) return;
		sta_bytSta = 2;

		_KillTimer(&g_objTimerMng, ACCOFF_DRVRRELOG_TIMER );
		_SetTimer(&g_objTimerMng, ACCOFF_DRVRRELOG_TIMER, ACCOFF_DRVRRELOG_INTERVAL, G_TmAccOffDriverReLog ); // 延时也是为了去抖
	}
}

/// 设置驾驶员身份请求: 
// 2客协议:   设置字（1）＋设置个数（1）＋【序号（1）＋编号（2）＋身份代码长度（1）＋身份代码】* N
// 千里眼协议:设置字（1）＋设置个数（1）＋【序号（1）＋编号（1）＋身份代码长度（1）＋身份代码 ＋驾驶证号长度（1）＋驾驶证号】* N
int CDriverCtrl::Deal3602( char* v_szSrcHandtel, char* v_szData, DWORD v_dwDataLen )
{
	tag2QDriverCfg objDriverCfg, objDriverCfgBkp;
	int iRet = 0;
	int iBufLen = 0;
	DWORD dw = 0;
	char buf[ SOCK_MAXSIZE ];
	BYTE bytResType = 0;
	BYTE bytSetType;
	BYTE bytSetCount = 0;
	BYTE bytNo = 0;
	BYTE bytLen = 0;
	BYTE byt = 0;

	//if( g_pView ) g_pView->DbgShowDataBuf( "Rcv3602", v_szData, v_dwDataLen );

	// 数据参数检查
	if( v_dwDataLen < 2 )
	{
		iRet = ERR_PAR;
		goto DEAL3602_FAILED;
	}

	// 读取原始配置
	iRet = ::GetSecCfg( &objDriverCfg, sizeof(objDriverCfg),
		offsetof(tagSecondCfg, m_uni2QDriverCfg.m_obj2QDriverCfg), sizeof(objDriverCfg) );
	if( iRet ) goto DEAL3602_FAILED;
	objDriverCfgBkp = objDriverCfg; // 先备份

	// 设置字
	bytSetType = BYTE( v_szData[0] );
	if( 0x7f == bytSetType ) bytSetType = 0;

	// 设置个数 (综合2客和千里眼协议，决定允许个数为0，表示只是清除)
	bytSetCount = BYTE( v_szData[1] );
	if( bytSetCount > sizeof(objDriverCfg.m_aryDriver) / sizeof(objDriverCfg.m_aryDriver[0]) )
	{
		iRet = ERR_PAR;
		goto DEAL3602_FAILED;
	}

	// 判断是否要清除原设置
	if( 1 == bytSetType )
	{
		memset( &objDriverCfg, 0, sizeof(objDriverCfg) );
	}

	// 更新配置
	dw = 2;
	for( byt = 0; byt < bytSetCount; byt ++ )
	{
		// 参数长度检查
		if( v_dwDataLen < dw + 4 )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}

		// 序号
		bytNo = BYTE( v_szData[ dw ] );
		if( 0x7f == bytNo ) bytNo = 0;
		if( bytNo >= sizeof(objDriverCfg.m_aryDriver) / sizeof(objDriverCfg.m_aryDriver[0]) )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}
		dw ++;

		// 编号
#if USE_PROTOCOL == 30
		memcpy( &objDriverCfg.m_aryDriver[bytNo].m_wDriverNo, v_szData + dw,
			sizeof(objDriverCfg.m_aryDriver[0].m_wDriverNo) );
		objDriverCfg.m_aryDriver[bytNo].m_wDriverNo = ::ntohs( objDriverCfg.m_aryDriver[bytNo].m_wDriverNo );
		if( 0 == objDriverCfg.m_aryDriver[bytNo].m_wDriverNo || 0xffff == objDriverCfg.m_aryDriver[bytNo].m_wDriverNo )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}
		dw += 2;
#endif
#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		objDriverCfg.m_aryDriver[bytNo].m_wDriverNo = BYTE( v_szData[dw] );
		if( 0 == objDriverCfg.m_aryDriver[bytNo].m_wDriverNo || objDriverCfg.m_aryDriver[bytNo].m_wDriverNo > 126 )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}
		dw ++;
#endif

		// 身份代码长度 （综合2客和千里眼协议，决定最大保存个数按较大的尺寸处理）
		bytLen = BYTE( v_szData[ dw ] );
		if( bytLen < 1 || bytLen > sizeof(objDriverCfg.m_aryDriver[0].m_szIdentID) / sizeof(objDriverCfg.m_aryDriver[0].m_szIdentID[0]) )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}
		dw ++;

		// 参数长度检查
		if( v_dwDataLen < dw + bytLen )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}

		// 保存身份代码
		strncpy( objDriverCfg.m_aryDriver[bytNo].m_szIdentID, v_szData + dw, bytLen );
		dw += bytLen;

#if USE_PROTOCOL == 0 || USE_PROTOCOL == 1 || USE_PROTOCOL == 2
		// 驾驶证号码长度
		bytLen = BYTE( v_szData[ dw ] );
		if( bytLen > sizeof(objDriverCfg.m_aryDriver[0].m_szDrivingLic) / sizeof(objDriverCfg.m_aryDriver[0].m_szDrivingLic[0]) )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}
		dw ++;

		// 参数长度检查
		if( v_dwDataLen < dw + bytLen )
		{
			iRet = ERR_PAR;
			goto DEAL3602_FAILED;
		}

		// 保存驾驶证号码
		strncpy( objDriverCfg.m_aryDriver[bytNo].m_szDrivingLic, v_szData + dw, bytLen );
		dw += bytLen;
#endif
	}

	// 发送应答
	bytResType = 0x01;
	iRet = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x36, 0x42, buf, sizeof(buf), iBufLen, CVT_8TO7 );
	if( !iRet )
	{
		// 写入文件
		if( iRet = ::SetSecCfg( &objDriverCfg, offsetof(tagSecondCfg, m_uni2QDriverCfg.m_obj2QDriverCfg),
			sizeof(objDriverCfg)) )
		{
			goto DEAL3602_FAILED;
		}

		// 080917,根据任务单080899修改,只允许报警数据、位置查询应答和短信通道下发设置的应答走短信通道
		if( iRet = g_objSock.SOCKSNDSMSDATA(buf, iBufLen, LV12, 0) )
		{
			::SetSecCfg( &objDriverCfgBkp, offsetof(tagSecondCfg, m_uni2QDriverCfg.m_obj2QDriverCfg),
				sizeof(objDriverCfgBkp) );
			goto DEAL3602_FAILED;
		}
		
		// 以下括号内的处理不做，因为若司机在开车，难道让司机此时输入代码，似乎比较危险
		// （检查当前的配置和已经登陆的司机信息是否一致，不一致则重新要求登陆）
	}
	else
		goto DEAL3602_FAILED;

	return 0;

DEAL3602_FAILED:
	bytResType = 0x7f;
	int iRet2 = g_objSms.MakeSmsFrame( (char*)&bytResType, 1, 0x36, 0x42, buf, sizeof(buf), iBufLen, CVT_8TO7 ); 
	// 080917,根据任务单080899修改,只允许报警数据、位置查询应答和短信通道下发设置的应答走短信通道
	if( !iRet2 ) g_objSock.SOCKSNDSMSDATA( buf, iBufLen, LV12, 0 );

	return iRet;
}

// 司机登陆应答
int CDriverCtrl::DealComu63( char* v_szData, DWORD v_dwDataLen )
{
	if( v_dwDataLen < 1 ) return ERR_PAR;

	if( 1 != v_szData[0] ) // 如果不是成功应答
	{
		g_objCarSta.SavDriverNo( 0xffff );
		g_objMonAlert.SetAlertDrive(true);

		// 播放请求司机登陆的提示
		char buf[100] = { 0 };
		buf[0] = 0x01;
		buf[1] = 0x00;
		strcat( buf, "司机朋友,您的身份代码不正确!" );
		DataPush( buf, strlen(buf), DEV_QIAN, DEV_DVR, 2 );
	}
	else if( v_dwDataLen < 3 ) // 如果是成功应答,但是输入参数不完整
	{
		g_objCarSta.SavDriverNo( 0xffff );
		g_objMonAlert.SetAlertDrive(true);

		char buf[100] = { 0 };
		buf[0] = 0x01;
		buf[1] = 0x00;
		strcat( buf, "司机朋友,您的身份代码不正确!" );
		DataPush( buf, strlen(buf), DEV_QIAN, DEV_DVR, 2 );
	}
	else
	{
		_KillTimer(&g_objTimerMng, DRIVERLOG_TIMER );
		WORD wDriverNo;
		memcpy( &wDriverNo, v_szData + 1, sizeof(wDriverNo) );
		wDriverNo = ntohs( wDriverNo );
		g_objCarSta.SavDriverNo( wDriverNo );
		g_objMonAlert.SetAlertDrive(false);

		char buf[100] = { 0 };
		buf[0] = 0x01;
		buf[1] = 0x00;
		strcat( buf, "司机朋友,验证通过,祝您旅途愉快!" );
		DataPush( buf, strlen(buf), DEV_QIAN, DEV_DVR, 2 );
	}

	g_objCarSta.SavDrvRecSta( false );

	return 0;
}

// 请求司机编号
int CDriverCtrl::DealComu65( char* v_szData, DWORD v_dwDataLen )
{
	tag2QDriverCfg objDriverCfg;
	objDriverCfg.Init();

	// 读取原始配置
	::GetSecCfg( &objDriverCfg, sizeof(objDriverCfg),
		offsetof(tagSecondCfg, m_uni2QDriverCfg.m_obj2QDriverCfg), sizeof(objDriverCfg) );

	// 查找当前司机编号的ID
	BYTE bytAnsType = 2;
	char szID[ sizeof(objDriverCfg.m_aryDriver[0].m_szIdentID) ];
	memset( szID, sizeof(szID), 0 );
	WORD wCurDriverNo = g_objCarSta.GetDriverNo();
	if( wCurDriverNo > 0 && wCurDriverNo < 0xffff )
	{
		for( size_t i = 0; i < sizeof(objDriverCfg.m_aryDriver) / sizeof(objDriverCfg.m_aryDriver[0]); i ++ )
		{
			if( objDriverCfg.m_aryDriver[i].m_wDriverNo == wCurDriverNo )
			{
				memcpy( szID, objDriverCfg.m_aryDriver[i].m_szIdentID, sizeof(szID) );
				bytAnsType = 1;
				break;
			}
		}
	}
	else
	{
		bytAnsType = 1;
	}

	char buf[ 64 ] = { 0 };
	buf[0] = 0x64;
	buf[1] = bytAnsType;
	buf[2] = wCurDriverNo / 0x100;
	buf[3] = wCurDriverNo % 0x100;
	buf[4] = BYTE( 0 == szID[sizeof(szID) - 1] ? strlen(szID) : sizeof(szID) );
	if( BYTE(buf[4]) < 8 ) memcpy( buf + 5, szID, BYTE(buf[4]) );
	DataPush( buf, 5 + BYTE(buf[4]), DEV_QIAN, DEV_DIAODU, 2 );

	return 0;
}

// 司机取消登陆应答
int CDriverCtrl::DealComu6c( char* v_szData, DWORD v_dwDataLen )
{
	return 0;
}

#endif
