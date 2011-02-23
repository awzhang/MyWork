#if !defined(IO_H_907BRTH34789SDFL)
#define IO_H_907BRTH34789SDFL

class CIO
{
public:
	CIO();
	virtual ~CIO();

	// 如下函数中需要进行互斥控制
	int IOGet( unsigned char v_ucIOSymb, unsigned char *v_p_ucIOSta );
	int IOSet( unsigned char v_ucIOSymb, unsigned char v_ucIOSta,  void* v_pReserv, unsigned int v_uiReservSiz );
	int IOCfgGet( unsigned char v_ucIOSymb, void *v_pIOCfg, DWORD v_dwCfgSize );
	int IOIntInit( unsigned char v_ucIOSymb, void* v_pIntFunc );
// 	int IOIntFuncSet( unsigned char v_ucIOSymb, void* v_pIntFunc );

private:
	int _Init(); // 需要互斥控制
	int _IntSet( unsigned char v_ucIOSymb, void* v_pIntFunc );
	int _SemP();
	int _SemV();
	int _SemCreate();
	int _ShmCreate();

private:
	int m_iSemID;
 	int m_iShmID;
 	char *m_pMemShare;
// 	bool m_bInit;
};



#endif
