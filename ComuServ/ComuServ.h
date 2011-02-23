#if !defined(COMUSERV_H_SJUS4EGA)
#define COMUSERV_H_SJUS4EGA

#include "MsgQueue.h"
#include "CfgMng.h"
#include "Gps.h"
#include "IO.h"

class CComuServ
{
public:
	CComuServ();
	virtual ~CComuServ();

//private:
	CGps m_objGps;
};


#endif