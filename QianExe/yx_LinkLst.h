#ifndef _YX_LINKLST_H_
#define _YX_LINKLST_H_

// 双向链表类

#define MAX_COUNT		1024	//最大链表项个数
#define enter_cs(cs)	pthread_mutex_lock(&cs)
#define leave_cs(cs)	pthread_mutex_unlock(&cs)

class LinkLst
{
private:

	struct Node {
		ulong len;
		struct Node *prev, *next;
	};
	ulong count;
	Node *head, *tail, *curnode;
	
	pthread_mutex_t cs;

public:
	LinkLst();
	~LinkLst();

	bool append(byte* data, ulong len);
	bool modify(byte* data, ulong len);
	bool insertb(byte* data, ulong len);
	bool inserta(byte* data, ulong len);
	bool remove();
	bool gohead();
	bool gotail();
	bool next();
	bool prev();
	ulong getdata(byte* out);
	ulong getcount();
	void reset();
};

#endif

