#include "yx_QianStdAfx.h"

//--------------------------------------------------------------------------------------------------------------------------
Queue::Queue()
{
	p1 = p2 = 0;
	for(int i=0; i<MAX_ITEM; i++) {
		lst[i].data = NULL;
		lst[i].len = 0;
	}

	pthread_mutex_init(&cs, NULL);
}

//--------------------------------------------------------------------------------------------------------------------------
Queue::~Queue()
{
	for(int i=0; i<MAX_ITEM; i++) {
		if(lst[i].len!=0)	{ free(lst[i].data); lst[i].data=NULL; }
	}

	pthread_mutex_destroy( &cs );
}


//--------------------------------------------------------------------------------------------------------------------------
// 将数据压入堆栈
// data: 待压入的数据指针
// len: 数据长度
// 返回: 是否压入成功
bool Queue::push(unsigned char *data, ulong len)
{
	pthread_mutex_lock(&cs);
	
	if(lst[p1].data==NULL && lst[p1].len==0) {
		lst[p1].data = (unsigned char*)malloc(len);
		if(lst[p1].data==NULL || lst[p1].data==(void*)0xcccccccc)	{
			pthread_mutex_unlock(&cs);
			return false;
		}
		lst[p1].len = len;
		memcpy(lst[p1].data, data, len);
		
		p1++;	
		if(p1>=MAX_ITEM)	p1=0;
		
		pthread_mutex_unlock(&cs);
		return true;	
	}
	
	pthread_mutex_unlock(&cs);
	return false;
}


//--------------------------------------------------------------------------------------------------------------------------
// 将数据从堆栈中推出
// data: 待推出的数据缓冲区指针
// 返回: 实际退出的数据字节数
ulong Queue::pop(unsigned char *data)
{
	pthread_mutex_lock(&cs);
	
	if(lst[p2].data!=NULL && lst[p2].len!=0) 
	{
		ulong retlen = lst[p2].len;
		
		memcpy(data, lst[p2].data, retlen);
		free(lst[p2].data);
		lst[p2].data = NULL;
		lst[p2].len = 0;
		
		p2++;
		if(p2>=MAX_ITEM)	p2=0;
		
		pthread_mutex_unlock(&cs);
		return retlen;
	} 
	
	pthread_mutex_unlock(&cs);
	return 0;
}


//--------------------------------------------------------------------------------------------------------------------------
// 获得堆栈的数据个数
// 返回: 数据个数
int Queue::getcount()
{
	int cnt = 0;
	pthread_mutex_lock(&cs);
	cnt = p1 - p2;
	if(cnt==0)	{
		if(lst[p1].data!=NULL && lst[p1].len!=0)	cnt = MAX_ITEM;
	}
	if(cnt<0)	cnt = cnt + MAX_ITEM;
	pthread_mutex_unlock(&cs);
	return cnt;
}


//--------------------------------------------------------------------------------------------------------------------------
// 清空所有堆栈(复位堆栈)
void Queue::reset()
{
	pthread_mutex_lock(&cs);
	for(int i=0; i<MAX_ITEM; i++) {
		if(lst[i].len!=0 && lst[i].data!=NULL) {
			free(lst[i].data);
			lst[i].data = NULL;
			lst[i].len = 0;
		}
	}
	p1 = p2 = 0;
	pthread_mutex_unlock(&cs);
}

