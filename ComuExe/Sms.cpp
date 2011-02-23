#include "ComuStdAfx.h"

#undef MSG_HEAD
#define MSG_HEAD ("ComuExe-Sms:")

#define DSTLEN_MAX 200

//----------------------------------------------------------------------------------------------------------------
// 可打印字符串转换为字节数据
// 如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// psrc: 源字符串指针
// pdst: 目标数据指针
// srclen: 源字符串长度
// 返回: 目标数据长度
// 100513 xun modify,若不修改，在转换奇数个字符时会有问题 (但是，不应该出现转换奇数个字符的情况）
int string_to_bytes(const char *psrc, uchar *pdst, int srclen)
{
	int i = 0;
	char *psrcfst = (char*)psrc;
	for( i=0; i<srclen; i+=2)
	{
		if( 0 == *psrc ) break;

		// 输出高4位
		if(*psrc>='0' && *psrc<='9')  {
			*pdst = (*psrc - '0') << 4;
		} else {
			*pdst = (*psrc - 'A' + 10) << 4;
		}

		if( i + 1 < srclen && 0 != *(psrc +1) )
		{
			psrc++;

			// 输出低4位
			if(*psrc>='0' && *psrc<='9') {
				*pdst |= *psrc - '0';
			} else {
				*pdst |= *psrc - 'A' + 10;
			}

			psrc++;
			pdst++;
		}
		else
		{
			*pdst &= 0xf0;
			i += 2;
			break;
		}
	}

	int iConvertedLen = int(psrc - psrcfst);
	if( iConvertedLen % 2 )
	{
		PRTMSG( MSG_ERR, "string_to_bytes err, converted srclen(%d) is odd!\n", iConvertedLen );
	}

	// 返回目标数据长度
	return i / 2;
}

//----------------------------------------------------------------------------------------------------------------
// 字节数据转换为可打印字符串
// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
// psrc: 源数据指针
// pdst: 目标字符串指针
// srclen: 源数据长度
// 返回: 目标字符串长度
static int bytes_to_string(const uchar *psrc, char *pdst, int srclen)
{
	const char tab[] = "0123456789ABCDEF";	// 0x0-0xf的字符查找表

	for(int i=0; i<srclen; i++) 
	{
		*pdst++ = tab[*psrc >> 4];		// 输出低4位
		*pdst++ = tab[*psrc & 0x0f];	// 输出高4位
		psrc++;
	}

	// 输出字符串加个结束符
	*pdst = '\0';

	// 返回目标字符串长度
	return srclen * 2;
}

//----------------------------------------------------------------------------------------------------------------
// 7bit编码
// psrc: 源字符串指针
// pdst: 目标编码串指针
// srclen: 源字符串长度
// 返回: 目标编码串长度
// 此编码可发送最多160个字符
static int encode_7bit(const char *psrc, uchar *pdst, int srclen)
{
	int src;		// 源字符串的计数值
	int dst;		// 目标编码串的计数值
	int ch;		// 当前正在处理的组内字符字节的序号，范围是0-7
	uchar left;	// 上一字节残余的数据
	
	// 计数值初始化
	src = 0;
	dst = 0;
	
	// 将源串每8个字节分为一组，压缩成7个字节
	// 循环该处理过程，直至源串被处理完
	// 如果分组不到8字节，也能正确处理
	while(src<srclen)
	{
		// 取源字符串的计数值的最低3位
		ch = src & 7;
		
		// 处理源串的每个字节
		if(ch == 0) {
			// 组内第一个字节，只是保存起来，待处理下一个字节时使用
			left = *psrc;
		} else {
			// 组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
			*pdst = (*psrc << (8-ch)) | left;
			// 将该字节剩下的左边部分，作为残余数据保存起来
			left = *psrc >> ch;
			// 修改目标串的指针和计数值
			pdst++;
			dst++;
		}
		
		// 修改源串的指针和计数值
		psrc++;
		src++;
	}
	
	// 返回目标串长度
	return dst;
}

//----------------------------------------------------------------------------------------------------------------
// 7bit解码
// psrc: 源编码串指针
// pdst: 目标字符串指针
// srclen: 源编码串长度
// 返回: 目标字符串长度
static int decode_7bit(const uchar *psrc, char *pdst, int srclen)
{
	int src;		// 源字符串的计数值
	int dst;		// 目标解码串的计数值
	int bytes;		// 当前正在处理的组内字节的序号，范围是0-6
	uchar left;		// 上一字节残余的数据
	
	// 计数值初始化
	src = 0;
	dst = 0;
	
	// 组内字节序号和残余数据初始化
	bytes = 0;
	left = 0;
	
	// 将源数据每7个字节分为一组，解压缩成8个字节
	// 循环该处理过程，直至源数据被处理完
	// 如果分组不到7字节，也能正确处理
	while( src < srclen )
	{
		// 将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
		*pdst = ((*psrc << bytes) | left) & 0x7f;
		// 将该字节剩下的左边部分，作为残余数据保存起来
		left = *psrc >> (7-bytes);
		// 修改目标串的指针和计数值
		pdst++;
		dst++;
		// 修改字节计数值
		bytes++;
		// 到了一组的最后一个字节
		if(bytes == 7) 
		{
			// 额外得到一个目标解码字节
			*pdst = left;
			// 修改目标串的指针和计数值
			pdst++;
			dst++;
			// 组内字节序号和残余数据初始化
			bytes = 0;
			left = 0;
		}
		// 修改源串的指针和计数值
		psrc++;
		src++;
	}

	// 输出字符串加个结束符
	*pdst = '\0';

	// 返回目标串长度
	return dst;
}

//----------------------------------------------------------------------------------------------------------------
// 8bit编码
// psrc: 源字符串指针
// pdst: 目标编码串指针
// srclen: 源字符串长度
// 返回: 目标编码串长度
static int encode_8bit(const char *psrc, uchar *pdst, int srclen)
{
	if(srclen<0)	return 0;
	// 简单复制
	memcpy(pdst, psrc, srclen);

	return srclen;
}

//----------------------------------------------------------------------------------------------------------------
// 8bit解码
// psrc: 源编码串指针
// pdst: 目标字符串指针
// srclen: 源编码串长度
// 返回: 目标字符串长度
static int decode_8bit(const uchar *psrc, char *pdst, int srclen)
{
	if(srclen<0)	return 0;
	
	// 简单复制
	memcpy(pdst, psrc, srclen);

	// 输出字符串加个结束符
	*pdst = '\0';

	return srclen;
}

//----------------------------------------------------------------------------------------------------------------
// UCS2编码
// psrc: 源字符串指针
// pdst: 目标编码串指针
// srclen: 源字符串长度
// 返回: 目标编码串长度
// 此编码可发送最多60个汉字
static size_t encode_ucs2(const char *psrc, uchar *pdst, size_t srclen)
{
	size_t iRet;
	size_t dstlen = DSTLEN_MAX;

	iconv_t eConv;
	eConv = iconv_open("UNICODEBIG","GB2312");

	if((iconv_t)(-1) == eConv )
	{
		perror("iconv_open()");
		return 0;
	}
	//que iconv:pdst为char
	//测试发现iRet返回的不是转换的长度，而是成功返回0，失败-1
	iRet = iconv(eConv, &psrc, &srclen, (char **)&pdst, &dstlen);
	if((size_t)(-1) == iRet) 
	{
		perror("iconv()");
		return 0;
	}

	iRet = iconv_close(eConv);
	if ( -1 == iRet ) 
	{
		perror("iconv_close()");
		return 0;
	}
	// 返回目标编码串长度
	return DSTLEN_MAX - dstlen;	//que size_t->int 是否需类型转换？
}

//----------------------------------------------------------------------------------------------------------------
// UCS2解码
// psrc: 源编码串指针
// pdst: 目标字符串指针
// srclen: 源编码串长度
// 返回: 字节数而不是返回字符数
static size_t decode_ucs2(const uchar *psrc, char *pdst, size_t srclen)
{
	size_t iRet;
	size_t dstlen = DSTLEN_MAX;			// UNICODE宽字符数目
//	const uchar* pTempSrc = psrc;
	char* pTempDst = pdst;

	iconv_t dConv;
	dConv = iconv_open("GB2312","UNICODEBIG");//GB18030?
	if((iconv_t)(-1) == dConv ) 
	{
		perror("iconv_open()");
		return 0;
	}

	if (*(psrc+1)==0 && srclen > 6 )
	{
		psrc = psrc+6;//表示为长短信
		srclen -= 6;
	}

	do
	{
		//psrc和pdst指针在iconv中会自动跳动,分别跳转源和目标转换的长度
		//测试发现部分unicode编码的字符无法转换，如‘-’（0x20 0x14），此处不能转换的字符用？填充
		iRet = iconv(dConv, (const char**)&psrc, &srclen, &pdst, &dstlen);
		if((size_t)(-1) == iRet)
		{
			perror("iconv()");
			//return 0;
			if ( srclen>=2 )
			{
				PRTMSG(MSG_DBG, "iconv transfer err data:%x %x", *psrc, *(psrc+1) );
				psrc+=2;
// 				*(pdst++) = 0xff;
// 				*(pdst++) = 0x1f;
 				*(pdst++) = 0xa1;
				*(pdst++) = 0xaa;//未识别以-代替
				srclen-=2;
				dstlen-=2;
			}
			
			continue;
		}
	}
	while( srclen>=2 );

	iRet = iconv_close(dConv);
	if ( -1 == iRet ) 
	{
		perror("iconv_close()");
		return 0;
	}

	// 输出字符串加个结束符
	pTempDst[DSTLEN_MAX - dstlen] = '\0';
	// 返回目标编码串长度
	return DSTLEN_MAX - dstlen;	//que size_t->int 是否需类型转换？
}

//----------------------------------------------------------------------------------------------------------------
// 正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数
// 如："8613851872468" --> "683158812764F8"
// psrc: 源字符串指针
// pdst: 目标字符串指针
// srclen: 源字符串长度
// 返回: 目标字符串长度
static int invert_numbers(const char *psrc, char *pdst, int srclen)
{
	int dstlen;		// 目标字符串长度
	char ch;		// 用于保存一个字符

	// 复制串长度
	dstlen = srclen;

	// 两两颠倒
	for(int i=0; i<srclen; i+=2)
	{
		ch = *psrc++;		// 保存先出现的字符
		*pdst++ = *psrc++;	// 复制后出现的字符
		*pdst++ = ch;		// 复制先出现的字符
	}

	// 源串长度是奇数吗？
	if(srclen & 1)
	{
		*(pdst-2) = 'F';	// 补'F'
		dstlen++;			// 目标串长度加1
	}

	// 输出字符串加个结束符
	*pdst = '\0';

	// 返回目标字符串长度
	return dstlen;
}

//----------------------------------------------------------------------------------------------------------------
// 两两颠倒的字符串转换为正常顺序的字符串
// 如："683158812764F8" --> "8613851872468"
// psrc: 源字符串指针
// pdst: 目标字符串指针
// srclen: 源字符串长度
// 返回: 目标字符串长度
static int serialize_numbers(const char *psrc, char *pdst, int srclen)
{
	int dstlen;			// 目标字符串长度
	char ch;			// 用于保存一个字符

	// 复制串长度
	dstlen = srclen;

	// 两两颠倒
	for(int i=0; i<srclen;i+=2)
	{
		ch = *psrc++;		// 保存先出现的字符
		*pdst++ = *psrc++;	// 复制后出现的字符
		*pdst++ = ch;		// 复制先出现的字符
	}

	// 最后的字符是'F'吗？
	if(*(pdst-1) == 'F')
	{
		pdst--;
		dstlen--;		// 目标字符串长度减1
	}

	// 输出字符串加个结束符
	*pdst = '\0';

	// 返回目标字符串长度
	return dstlen;
}

//v_pLen:编码后，v_pBuf所含的字节数，主要是因为UCS2编码时不能用strlen来算字符数
bool evdo_encode_pdu(SM_PARAM *v_pSM,char *v_pBuf,int &v_pLen)
{
	
	if (NULL==v_pSM || NULL==v_pBuf)
	{
		return false;
	}
	int iUdLen = strlen(v_pSM->ud);
	if( iUdLen > sizeof(v_pSM->ud) - 1 ) iUdLen = sizeof(v_pSM->ud) - 1;

// 	/// 临时测试发送含0字符短信
// 	{
// 		for( int i = iUdLen; i > 5; i ++ )
// 		{
// 			v_pSM->ud[i] = v_pSM->ud[i-1];
// 		}
// 		v_pSM->ud[5] = 0;
// 		iUdLen ++;
// 	}

	switch(v_pSM->dcs)
	{
	case SMS_7BIT:
		{
			//v_pLen = encode_7bit(v_pSM->ud,(uchar*)v_pBuf,iUdLen + 1);
			memcpy( v_pBuf, v_pSM->ud, iUdLen );
			v_pLen = iUdLen;
		}
		break;
		
	case SMS_UCS2:
		{
			//que 为什么要用uchar格式？
			v_pLen = encode_ucs2(v_pSM->ud,(uchar*)v_pBuf,iUdLen);
		}
		break;
		
	case SMS_8BIT:
		{
			v_pLen = encode_8bit(v_pSM->ud,(uchar*)v_pBuf,iUdLen);
		}
		break;
		
	default:
		{
			PRTMSG(MSG_ERR,"UnSupported Character Set");
			return false;
		}
		
	}
	
	return true;
}

//psrc:指向短信的用户数据
bool evdo_decode_pdu(const char *psrc, SM_PARAM *pdst)//que 接收的消息长度参数，需添加
{
	if (NULL==psrc || NULL==pdst)
	{
		return false;
	}

	size_t dstLen;
	uchar buf[256] = {0};
	memcpy(buf,psrc,pdst->udl);
	
	switch(pdst->dcs)
	{
	case SMS_7BIT:
		{
			//pdst->UDL = cdma_decode_7bit(buf,pdst->UD,pdst->UDL);
			memcpy(pdst->ud,psrc,pdst->udl);
			dstLen = pdst->udl;
		}
		break;
		
	case SMS_UCS2:
		{
//			dstLen = decode_ucs2(buf,pdst->ud,pdst->udl*2);//接收的消息长度
			dstLen = decode_ucs2(buf,pdst->ud,pdst->udl);//接收的消息长度
		}
		break;
		
	case SMS_8BIT:
		{
			dstLen = decode_8bit(buf,pdst->ud,pdst->udl);
		}
		break;
		
	default:
		PRTMSG(MSG_ERR,"rcv unknown format SM\n");
		break;
	}
	
	return dstLen;
}


//v_pLen:编码后，v_pBuf所含的字节数，主要是因为UCS2编码时不能用strlen来算字符数
bool wcdma_encode_pdu(SM_PARAM *v_pSM,char *v_pBuf,int &v_pLen)
{
	
	if (NULL==v_pSM || NULL==v_pBuf)
	{
		return false;
	}
	
	switch(v_pSM->dcs)
	{
	case SMS_7BIT:
		{
			v_pLen = encode_7bit(v_pSM->ud,(uchar*)v_pBuf,strlen(v_pSM->ud));
		}
		break;
		
	case SMS_UCS2:
		{
			//que 为什么要用uchar格式？
			v_pLen = encode_ucs2(v_pSM->ud,(uchar*)v_pBuf,strlen(v_pSM->ud));
		}
		break;
		
	case SMS_8BIT:
		{
			v_pLen = encode_8bit(v_pSM->ud,(uchar*)v_pBuf,strlen(v_pSM->ud));
		}
		break;
		
	default:
		{
			PRTMSG(MSG_ERR,"UnSupported Character Set");
			return false;
		}
		
	}
	
	return true;
}

bool wcdma_decode_pdu(const char *psrc, SM_PARAM *pdst)//que 接收的消息长度参数，需添加
{
	if (NULL==psrc || NULL==pdst)
	{
		return false;
	}
	
	size_t dstLen;
	uchar buf[256] = {0};
	memcpy(buf,psrc,pdst->udl);
	
	switch(pdst->dcs)
	{
	case SMS_7BIT:
		{
			//pdst->UDL = cdma_decode_7bit(buf,pdst->UD,pdst->UDL);
			memcpy(pdst->ud,psrc,pdst->udl);
			dstLen = pdst->udl;
		}
		break;
		
	case SMS_UCS2:
		{
			//			dstLen = decode_ucs2(buf,pdst->ud,pdst->udl*2);//接收的消息长度
			dstLen = decode_ucs2(buf,pdst->ud,pdst->udl);//接收的消息长度
		}
		break;
		
	case SMS_8BIT:
		{
			dstLen = decode_8bit(buf,pdst->ud,pdst->udl);
		}
		break;
		
	default:
		PRTMSG(MSG_ERR,"rcv unknown format SM\n");
		break;
	}
	
	return dstLen;
}

//----------------------------------------------------------------------------------------------------------------
// PDU编码，用于编制、发送短消息
// psrc: 源PDU参数指针
// pdst: 目标PDU串指针
// 返回: 目标PDU串长度
int encode_pdu(SM_PARAM *psrc, char *pdst)
{
	int len=0;			// 内部用的串长度
	int dstlen=0;			// 目标PDU串长度
	uchar buf[256]={0};	// 内部用的缓冲区

	// SMSC地址信息段(dkm del)
//	len = strlen(psrc->sca);	// SMSC地址字符串的长度	
//	buf[0] = (char)((len & 1) == 0 ? len : len + 1) / 2 + 1;	// SMSC地址信息长度
//	buf[1] = 0x91;		// 固定: 用国际格式号码
//	dstlen = bytes_to_string(buf, pdst, 2);		// 转换2个字节到目标PDU串
//	dstlen += invert_numbers(psrc->sca, &pdst[dstlen], len);	// 转换SMSC号码到目标PDU串

	// TPDU段基本参数、目标地址等
	len = strlen(psrc->tpa);		// TP-DA地址字符串的长度

	buf[0] = 0x00;					// (dkm add)
	buf[1] = 0x11;					// 是发送短信(TP-MTI=01)，TP-VP用相对格式(TP-VPF=10)
	buf[2] = 0x00;					// TP-MR=0
	buf[3] = (char)len;				// 目标地址数字个数(TP-DA地址字符串真实长度)
	buf[4] = 0x91;					// 固定: 用国际格式号码
	dstlen = bytes_to_string(buf, pdst, 5);		// 转换4个字节到目标PDU串
	dstlen += invert_numbers(psrc->tpa, &pdst[dstlen], len);	// 转换TP-DA到目标PDU串

	// 确定用户信息的编码方式 (dkm add)
	bool unicode = false;
	for(size_t i=0; i<strlen(psrc->ud); i++) {
		if((byte)psrc->ud[i] >= 0x80)	  { unicode=true;  break; }
	}
	if(unicode)		psrc->dcs = SMS_UCS2;
	else			psrc->dcs = SMS_7BIT;

	// TPDU段协议标识、编码方式、用户信息等
	len = strlen(psrc->ud);		// 用户信息字符串的长度
// 	{
//		/// 临时测试字符0是否可以发短信
// 		for( size_t i = len; i > 5; i -- )
// 		{
// 			psrc->ud[i] = psrc->ud[i - 1];
// 		}
// 		psrc->ud[5] = 0;
// 		len ++;
// 	}
	if( len > sizeof(psrc->ud) - 1 ) len = sizeof(psrc->ud) - 1;
	buf[0] = psrc->pid;			// 协议标识(TP-PID)
	buf[1] = psrc->dcs;			// 用户信息编码方式(TP-DCS)
	buf[2] = 0;					// 有效期(TP-VP)为5分钟
	if(psrc->dcs == SMS_7BIT)	
	{
		// 7-bit编码方式
		buf[3] = len;			// 编码前长度
		len = encode_7bit(psrc->ud, &buf[4], len+1) + 4;	// 转换TP-DA到目标PDU串 (转换内容要多一个结束符0，否则可能会丢失内容)
	}
	else if(psrc->dcs == SMS_UCS2)
	{
		// UCS2编码方式
		buf[3] = encode_ucs2(psrc->ud, &buf[4], len);	// 转换TP-DA到目标PDU串
		len = buf[3] + 4;		// len等于该段数据长度
	}
	else
	{
		// 8-bit编码方式
		buf[3] = encode_8bit(psrc->ud, &buf[4], len);	// 转换TP-DA到目标PDU串
		len = buf[3] + 4;		// len等于该段数据长度
	}
	dstlen += bytes_to_string(buf, &pdst[dstlen], len);		// 转换该段数据到目标PDU串
	
	// 返回目标字符串长度
	return dstlen;
}




//----------------------------------------------------------------------------------------------------------------
// PDU解码，用于接收、阅读短消息
// psrc: 源PDU串指针
// pdst: 目标PDU参数指针
// 返回: 用户信息串长度
int decode_pdu(const char *psrc, SM_PARAM *pdst)
{
	int dstlen = 0;			// 目标PDU串长度
	uchar tmp;			// 内部用的临时字节变量
	uchar buf[256] = { 0 };		// 内部用的缓冲区

	// SMSC地址信息段
	string_to_bytes(psrc, &tmp, 2);	// 取长度
	tmp = (tmp - 1) * 2;	// SMSC号码串长度
	psrc += 4;			// 指针后移，忽略了SMSC地址格式
	serialize_numbers(psrc, pdst->sca, tmp);	// 转换SMSC号码到目标PDU串
	psrc += tmp;		// 指针后移

	// TPDU段基本参数、回复地址等
	string_to_bytes(psrc, &tmp, 2);	// 取基本参数
	psrc += 2;		// 指针后移

	//if(tmp & 0x04)
	if(tmp)
	{
		// 包含回复地址，取回复地址信息
		string_to_bytes(psrc, &tmp, 2);	// 取长度
		if(tmp & 1) tmp += 1;	// 调整奇偶性
		psrc += 4;			// 指针后移，忽略了回复地址(TP-RA)格式
		serialize_numbers(psrc, pdst->tpa, tmp);	// 取TP-RA号码
		psrc += tmp;		// 指针后移
		PRTMSG(MSG_DBG, "SM from:%.16s", pdst->tpa);
	}

	// TPDU段协议标识、编码方式、用户信息等
	string_to_bytes(psrc, (uchar*)&pdst->pid, 2);	// 取协议标识(TP-PID)
	psrc += 2;		// 指针后移
	string_to_bytes(psrc, (uchar*)&pdst->dcs, 2);	// 取编码方式(TP-DCS)
	psrc += 2;		// 指针后移
	serialize_numbers(psrc, pdst->scts, 14);		// 服务时间戳字符串(TP_SCTS) 
	psrc += 14;		// 指针后移
	string_to_bytes(psrc, &tmp, 2);	// 用户信息长度(TP-UDL)
	psrc += 2;		// 指针后移
	
	if( (pdst->dcs&0x0c)/4==0 )		
	{
		// 7-bit解码
		dstlen = string_to_bytes(psrc, buf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4);	// 格式转换
		decode_7bit(buf, pdst->ud, dstlen);	// 转换到TP-DU
		dstlen = tmp;
	}
	else if( (pdst->dcs&0x0c)/4==2 )
	{
		// UCS2解码
		dstlen = string_to_bytes(psrc, buf, tmp * 2);	// 格式转换
// 		PRTMSG(MSG_DBG, "testc sms ud:");
// 		PrintString((char*)buf,dstlen);
		memset(pdst->ud,0,sizeof(pdst->ud));
		dstlen = decode_ucs2(buf, pdst->ud, dstlen);	// 转换到TP-DU
	}
	else if( (pdst->dcs&0x0c)/4==1 )
	{
		// 8-bit解码
		dstlen = string_to_bytes(psrc, buf, tmp * 2);	// 格式转换
		dstlen = decode_8bit(buf, pdst->ud, dstlen);	// 转换到TP-DU
	}

	// 返回目标字符串长度
	return dstlen;
}


