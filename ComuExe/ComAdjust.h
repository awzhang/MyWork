#ifndef _COMADJUST_H_
#define _COMADJUST_H_

class CComAdjust
{
public:
	CComAdjust();
	virtual ~CComAdjust();

	int Init();
	int Release();

	bool ComOpen();
	int ComClose();
	int ComWrite(char *v_szBuf, int v_iLen);

	void P_ThreadComAdjustRead();

	int  DetranData(char *v_szBuf, int v_iLen);
	void AnalyseComFrame(char *v_bytBuf, int v_iLen);

	bool check_crc(const byte *buf, const int len);
	byte get_crc(const byte *buf, const int len);

private:
	int			m_iComPort;		//´®¿Ú¾ä±ú
	pthread_t	m_pthreadRead;

	char		m_szComFrameBuf[1024];
	int			m_iComFrameLen;

	pthread_mutex_t		m_MutexWrite;
};

#endif

