#include "yx_QianStdAfx.h"

#if USE_LIAONING_SANQI == 1

void *G_ThreadApplicationRecv(void *arg)
{
	g_objApplication.P_ThreadRecv();
}

//////////////////////////////////////////////////////////////////////////

#undef MSG_HEAD
#define MSG_HEAD	("QianExe-LN_App")

CLN_Application::CLN_Application()
{
	m_objRecvDataMng.InitCfg(10*1024, 30*1000);
}

CLN_Application::~CLN_Application()
{

}

int CLN_Application::Init()
{
	//创建线程
	pthread_t threadRecv;
	if( pthread_create(&threadRecv, NULL, G_ThreadApplicationRecv, NULL) != 0	)
	{
		PRTMSG(MSG_ERR, "create Application thread failed!\n");
	}
}

int CLN_Application::Release()
{

}

int CLN_Application::MakeAppFrame(char v_szCmd, short v_shAction, char *v_szSrcBuf, DWORD v_dwSrcLen, char *v_szDesBuf, DWORD v_dwDesSize, DWORD &v_dwDesLen)
{
	// 目标缓冲区长度检查
	if( v_dwDesSize < v_dwSrcLen + 3 )
		return ERR_BUFLACK;

	v_dwDesLen = 0;
	v_szDesBuf[v_dwDesLen++] = v_szCmd;
	
	G_LocalToNet((char*)&v_shAction, v_szDesBuf+v_dwDesLen, 2);
	v_dwDesLen += 2;

	memcpy(v_szDesBuf+v_dwDesLen, v_szSrcBuf, v_dwSrcLen);
	v_dwDesLen += v_dwSrcLen;

	return 0;
}

int CLN_Application::SendAppFrame(char v_szModeType, char v_szControlByte, bool v_bUseLocalSEQ, DWORD v_dwCenterSEQ, char *v_szData, DWORD v_dwDataLen)
{
	char  szBuf[UDP_SENDSIZE] = {0};
	DWORD dwLen = 0;
	DWORD dwPacket = 0;

	szBuf[dwLen++] = v_szModeType;					// 传输模式
	szBuf[dwLen++] = v_szControlByte;				// 控制字
	szBuf[dwLen++] = (v_bUseLocalSEQ ? 1 : 0);		// 是否使用本地SEQ流水号
	memcpy(szBuf+dwLen, (void*)&v_dwCenterSEQ, 4);	// 给中心应答时使用的SEQ流水号
	dwLen += 4;
	memcpy(szBuf+dwLen, v_szData, v_dwDataLen);		// 应用层数据
	dwLen += v_dwDataLen;

	//将数据帧推入传输层的发送队列
	g_objTransport.m_objSendDataMng.PushData(LV2, dwLen, szBuf, dwPacket);
}

void CLN_Application::P_ThreadRecv()
{
	char  szPopBuf[UDP_SENDSIZE] = {0};
	DWORD dwPopLen = 0;
	byte  bytLvl = 0;
	DWORD dwPushTm = 0;

	DWORD dwCenterSEQ = 0;

	char  szTemBuf[2] = {0};
	short shAction = 0;
	
	while( !g_bProgExit )
	{
		usleep(100000);
		if( !m_objRecvDataMng.PopData(bytLvl, sizeof(szPopBuf), dwPopLen, szPopBuf, dwPushTm) )
		{
			// SEQ流水号
			memcpy((void*)&dwCenterSEQ, szPopBuf, 4);

			// 动作类型
			szTemBuf[0] = szPopBuf[6];
			szTemBuf[1] = szPopBuf[5];
			memcpy((void*)&shAction, szTemBuf, 2);

			switch(szPopBuf[4])
			{
			case 0x01:	// 终端控制动作
				{
					switch(shAction)
					{
					case 0x0001:	// 终端初始化
						g_objLNConfig.Deal01_0001(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;

					case 0x0002:	// 点名
						g_objLNConfig.Deal01_0002(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;

					case 0x0004:	// 油路开关
						g_objLNConfig.Deal01_0004(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;

					case 0x0006:	// 监听
						g_objLNConfig.Deal01_0006(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0007:	// GPS开关
						g_objLNConfig.Deal01_0007(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					default:
						break;
					}					
				}
				break;

			case 0x02:	// 外设控制动作
				{
					switch(shAction)
					{
					case 0x0002:	// 
						g_objLNPhoto.Deal02_0002(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0003:	// 
						g_objLNPhoto.Deal02_0003(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					default:
						break;
					}					
				}
				break;

			case 0x03:	// 外设数据发送
				break;

			case 0x04:	// 外设数据获取
				{
					switch(shAction)
					{
					case 0x0001:	// 
						g_objLNPhoto.Deal04_0001(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0002:	// 
						g_objLNPhoto.Deal04_0002(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					default:
						break;
					}					
				}
				break;

			case 0x05:	// 终端控制动作
				break;

			case 0x06:	// 外设属性查询
				break;

			case 0x07:	// 终端状态查询
				break;
				
			case 0x08:	// 外设状态查询
				break;

			case 0x09:	// 终端设置
				{
					switch(shAction)
					{
					case 0x0001:	// 里程设置
						g_objLNConfig.Deal09_0001(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
						
					case 0x0008:	// 本机电话号码设置
						g_objLNConfig.Deal09_0008(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
						
					case 0x0009:	// 呼叫限制设置
						g_objLNConfig.Deal09_0009(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
						
					case 0x0010:	// 连接参数设置
						g_objLNConfig.Deal09_0010(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;

					case 0x0011:	// 心跳间隔设置
						g_objLNConfig.Deal09_0011(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
						
					case 0x0012:	// 报告参数设置
						g_objLNConfig.Deal09_0012(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
						
					default:
						break;
					}
				}
				break;
				
			case 0x0a:	// 终端设置查询
				break;

			case 0x0b:	// 外设设置
				break;
				
			case 0x0c:	// 外设设置查询
				break;

			case 0x0d:	// 主动位置上报规则设置
				{
					g_objLNConfig.Deal0D_0000(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
				}
				break;
				
			case 0x0e:	// 主动位置上报规则移除
				{
					g_objLNConfig.Deal0E_0000(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
				}
				break;

			case 0x11:	// 报告触发条件设置
				{
					switch(shAction)
					{
					case 0x0001: //紧急报告设置
							g_objLNConfig.Deal11_0001(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;	
					case 0x0002: //里程报告设置
							g_objLNConfig.Deal11_0002(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;	
					case 0x0003:	
							g_objLNConfig.Deal11_0003(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0005: //超时停车报告设置
							g_objLNConfig.Deal11_0005(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0006:	// 区域报告设置
							g_objLNConfig.Deal11_0006(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0007:	// 速度异常设置
							g_objLNConfig.Deal11_0007(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0008: //时间段速度异常报告设置
							g_objLNConfig.Deal11_0008(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0009: //偏航报告设置
							g_objLNConfig.Deal11_0009(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x000e:	//主电源断电报告
							g_objLNConfig.Deal11_000E(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x000f:	// 点火报告设置
							g_objLNConfig.Deal11_000F(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;	
					case 0x0014: //疲劳报告设置
							g_objLNConfig.Deal11_0014(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0016: //分时段区域超速报告设置
							g_objLNConfig.Deal11_0016(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0018: //分时段偏航报告设置
							g_objLNConfig.Deal11_0018(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0019: //区域超时停车报告设置
							g_objLNConfig.Deal11_0019(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0022:	//进区域报告设置
							g_objLNConfig.Deal11_0022(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0023: //出区域报告设置
							g_objLNConfig.Deal11_0023(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0024: //区域内报告设置
							g_objLNConfig.Deal11_0024(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0025: //区域外报告设置
							g_objLNConfig.Deal11_0025(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0026: //区域内超速报告设置
							g_objLNConfig.Deal11_0026(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0027: //区域外超速报告设置
							g_objLNConfig.Deal11_0027(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0028: //区域内超时停车报告设置
							g_objLNConfig.Deal11_0028(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;	
					case 0x0029: //
							g_objLNConfig.Deal11_0029(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;	
					default:
						break;
					}
				}
				break;
				
			case 0x12:	// 报告触发条件移除
				{
					g_objLNConfig.Deal12(szPopBuf+5, dwPopLen-5, dwCenterSEQ);
				}
				break;

			case 0x15:	// 报告处理
				{
					g_objLNConfig.Deal15(szPopBuf+5, dwPopLen-5, dwCenterSEQ);

				}
				break;

			case 0x1a:	// 消息下发
				break;

			case 0x1b:	// 其他
				{
					switch(shAction)
					{
					case 0x0001: // 远程升级
						g_objTransport.Deal1B_0001(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					case 0x0002: // 远程升级数据
						g_objTransport.Deal1B_0002(szPopBuf+7, dwPopLen-7, dwCenterSEQ);
						break;
					default:
						break;
					}
				}
				break;

			case 0x41:	// 终端
				{
					switch(shAction)
					{
					case 0x0000:	// 自注册应答
						g_objLogin.RegisterAnswer( szPopBuf+7, dwPopLen-7 );
						break;

					case 0x0001:	// 登陆应答
						g_objLogin.LoginAnswer( szPopBuf+7, dwPopLen-7 );
						break;

					case 0x0003:	// 心跳数据
						g_objLogin.HeartKeepAnswer( szPopBuf+7, dwPopLen-7 );
						break;

					case 0x0004:	// 自检请求
						break;

					default:
						break;
					}
				}
				break;
			case 0x42:	// 
				{
					switch(shAction)
					{
						case 0x0004:	// 
						g_objLNPhoto.Deal42_0004(szPopBuf+7, dwPopLen-7, dwCenterSEQ);				
						break;

					default:
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

#endif

