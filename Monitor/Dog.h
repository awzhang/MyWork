#ifndef _DOG_H_
#define _DOG_H_

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux specific) */
};

extern DWORD DOG_RSTVAL;

class CDog  
{
public:
	CDog();
	virtual ~CDog();

public:
	//void DogClr( DWORD v_dwSymb );
	DWORD DogQuery();
	void DogInit();

private:
	int _SemCreate();
	int _SemV();
	int _SemP();
	int _ShmCreate();

private:
	int m_iSemID;
	int m_iShmID;
	char* m_pShareMem;
};

#endif
