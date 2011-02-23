#include "ComuStdAfx.h"

//////////////////////////////////////////////////////////////////////////
StackLst::StackLst()
{
	p1 = 0;
	p2 = 0;
	for(int i=0; i<MAX_ITEM; i++) {
		lst[i].data = NULL;
		lst[i].len = 0;
	}
	pthread_mutex_init(&cs, NULL);
}

//--------------------------------------------------------------------------------------------------------------------------
StackLst::~StackLst()
{
	for(int i=0; i<MAX_ITEM; i++) {
		if(lst[i].len!=0)	{ free(lst[i].data); lst[i].data=NULL; }
	}

	pthread_mutex_destroy(&cs);
}


//--------------------------------------------------------------------------------------------------------------------------
bool StackLst::push(unsigned char *data, ulong len)
{
	pthread_mutex_lock(&cs);
	if(lst[p1].data==NULL && lst[p1].len==0) 
	{
		lst[p1].data = (unsigned char*)malloc(len);

		if(lst[p1].data==NULL || lst[p1].data==(void*)0xcccccccc)
		{
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
ulong StackLst::pop(unsigned char *data)
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
int StackLst::getcount()
{
	int cnt = 0;
	pthread_mutex_lock(&cs);
	
	cnt = p1 - p2;
	if(cnt==0)
	{
		if(lst[p1].data!=NULL && lst[p1].len!=0)
			cnt = MAX_ITEM;
	}
	if(cnt<0)	cnt = cnt + MAX_ITEM;
	
	pthread_mutex_unlock(&cs);
	return cnt;
}


//--------------------------------------------------------------------------------------------------------------------------
void StackLst::reset()
{
	pthread_mutex_lock(&cs);
	
	for(int i=0; i<MAX_ITEM; i++) 
	{
		if(lst[i].len!=0 && lst[i].data!=NULL)	
		{
			free(lst[i].data);
			lst[i].data = NULL;
			lst[i].len = 0;
		}
	}
	
	pthread_mutex_unlock(&cs);
}


