#ifndef _YX_FLASHPART4MNG_H_
#define _YX_FLASHPART4MNG_H_

class CFlashPart4Mng  
{
public:
	CFlashPart4Mng();
	virtual ~CFlashPart4Mng();
	
	void Init();
	void DelOldFile();
	DWORD GetToTalFileSize();
	void ChkFlash();	
};

#endif


