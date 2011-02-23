#ifndef _YX_STACKLST_H_
#define _YX_STACKLST_H_

#define MAX_ITEM 	1024

class StackLst
{
private:
	
	struct Item {
		unsigned char *data;
		ulong len;
	};
	
	Item lst[MAX_ITEM];
	int p1,p2;

	pthread_mutex_t cs;

public:
	StackLst();
	~StackLst();
	bool  push(unsigned char *data, ulong len);
	ulong pop(unsigned char *data);
	int   getcount();
	void  reset();
};

#endif 

