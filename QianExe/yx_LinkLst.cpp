#include "yx_QianStdAfx.h"

LinkLst::LinkLst()
{
	count = 0;
	head = tail = curnode = NULL;
	
	pthread_mutex_init(&cs, NULL);
}

LinkLst::~LinkLst()
{
	Node *node = head;
	Node *next;
	while(node!=tail) {
		next = node->next;
		if(node)	{ free(node); node=NULL; }
		node = next;
	}
	if(tail)	{ free(tail); tail=NULL; }
	count = 0;
	head = tail = curnode = NULL;

	pthread_mutex_destroy(&cs);
}

//在链表尾添加一个链表数据
//data: 数据指针
//len: 数据长度
bool LinkLst::append(byte* data, ulong len)
{
	if(data==NULL || len==0)	return false;
	
	enter_cs(cs);
	if(count>MAX_COUNT)		{ leave_cs(cs); return false; }
	Node *node = (Node*)malloc(sizeof(Node)+len);
	if(node==NULL)		{ leave_cs(cs); return false; }
	node->len = len;
	memcpy((byte*)node+sizeof(Node), data, len);
	
	node->prev = tail;
	if(count==0) { head = node; curnode = head; } 
	else { tail->next = node; }
	
	node->next = NULL;
	tail = node;
	count++;
	
	leave_cs(cs);
	
	return true;
}

//修改当前链表数据
//data: 数据指针
//len: 数据长度	
bool LinkLst::modify(byte* data, ulong len)
{
	enter_cs(cs);
	if(count==0)	{ leave_cs(cs); return false; }
	if(curnode==NULL)	{ leave_cs(cs); return false; }
	
	Node *node = (Node*)malloc(sizeof(Node)+len);
	if(node==NULL)	{ leave_cs(cs); return false; }
	
	Node *prev = curnode->prev;
	Node *next = curnode->next;
	
	node->len = len;
	node->prev = prev;
	node->next = next;
	if(prev!=NULL)		prev->next = node;
	if(next!=NULL)		next->prev = node;
	
	memcpy((byte*)node+sizeof(Node), data, len);
	
	if(head==curnode)	head = node;
	if(tail==curnode)	tail = node;
	
	free(curnode);
	curnode = NULL;
	curnode = node;
	
	leave_cs(cs);
	return true;
}

//在当前链表前添加一个链表数据
//data: 数据指针
//len: 数据长度
bool LinkLst::insertb(byte* data, ulong len)
{
	enter_cs(cs);
	
	if(count>MAX_COUNT)		{ leave_cs(cs); return false; }
	if(count==0)			{ leave_cs(cs); return false; }
	if(curnode==NULL)		{ leave_cs(cs); return false; }
	
	Node *node = (Node*)malloc(sizeof(Node)+len);
	if(node==NULL)	{ leave_cs(cs); return false; }
	node->len = len;
	memcpy((byte*)node+sizeof(Node), data, len);
	
	node->next = curnode;
	node->prev = curnode->prev;
	if(curnode->prev==NULL) { head = node; } 
	else { curnode->prev->next = node; }
	curnode->prev = node;
	count++;
	leave_cs(cs);
	return true;
}

//在当前链表后添加一个链表数据
//data: 数据指针
//len: 数据长度
bool LinkLst::inserta(byte* data, ulong len)
{
	enter_cs(cs);
	
	if(count>MAX_COUNT)		{ leave_cs(cs); return false; }
	if(count==0)			{ leave_cs(cs); return false; }
	if(curnode==NULL)		{ leave_cs(cs); return false; }
	
	Node *node = (Node*)malloc(sizeof(Node)+len);
	if(node==NULL)	{ leave_cs(cs); return false; }
	node->len = len;
	memcpy((byte*)node+sizeof(Node), data, len);
	
	node->next = curnode->next;
	node->prev = curnode;
	if(curnode->next==NULL) { tail = node; }
	else { curnode->next->prev = node; }
	curnode->next = node;
	count++;
	leave_cs(cs);
	return true;
}

//移除当前链表数据
bool LinkLst::remove()
{
	enter_cs(cs);
	
	if(count==0)	{ leave_cs(cs); return false; }
	if(curnode==NULL)	{ leave_cs(cs); return false; }
	
	Node *prevnode, *nextnode, *cur;
	prevnode = curnode->prev;
	nextnode = curnode->next;
	
	if(prevnode==NULL) { head = nextnode; cur = head; } 
	else { prevnode->next = nextnode; cur = nextnode; }
	
	if(nextnode==NULL) { tail = prevnode; cur = tail; } 
	else { nextnode->prev = prevnode; cur = nextnode; }
	
	free(curnode);
	curnode = NULL;
	curnode = cur;
	count--;
	
	leave_cs(cs);
	return true;
}

//移动链表指针到链表头
bool LinkLst::gohead()
{
	enter_cs(cs);
	if(count!=0 && curnode!=NULL) { curnode = head; leave_cs(cs); return true; } 
	else { leave_cs(cs); return false; }
}

//移动链表指针到链表尾	
bool LinkLst::gotail()
{
	enter_cs(cs);
	if(count!=0 && curnode!=NULL) { curnode = tail; leave_cs(cs); return true; } 
	else { leave_cs(cs); return false; }
}

//移动链表指针到下一链表
bool LinkLst::next()
{
	enter_cs(cs);
	if(count==0)		{ leave_cs(cs); return false; }
	if(curnode==NULL)	{ leave_cs(cs); return false; }
	if(curnode->next==NULL)		{ leave_cs(cs); return false; }
	curnode = curnode->next;
	leave_cs(cs);
	return true;	 
}

//移动链表指针到上一链表	
bool LinkLst::prev()
{
	enter_cs(cs);
	if(count==0)		{ leave_cs(cs); return false; }
	if(curnode==NULL)	{ leave_cs(cs); return false; }
	if(curnode->prev==NULL)		{ leave_cs(cs); return false; }
	curnode = curnode->prev;
	leave_cs(cs);
	return true;
}

//获得当前链表数据
//out: 数据输出指针
//返回: 数据输出长度
ulong LinkLst::getdata(byte* out)
{
	if(out==NULL)	return 0;
	enter_cs(cs);
	if(count==0)		{ leave_cs(cs); return 0; }
	if(curnode==NULL)	{ leave_cs(cs); return 0; }
	
	int len = curnode->len;
	memcpy(out, (byte*)curnode+sizeof(Node), len);
	leave_cs(cs);
	return len;
}

//获得链表个数
//返回: 链表个数
ulong LinkLst::getcount()
{
	enter_cs(cs);
	ulong cnt = count;
	leave_cs(cs);
	return cnt;
}

//清空所有链表
void LinkLst::reset()
{
	enter_cs(cs);
	Node *node = head;
	Node *next;
	while(node!=tail) {
		next = node->next;
		if(node) { free(node); node=NULL; }
		node = next;
	}
	if(tail)	{ free(tail); tail=NULL; }
	count = 0;
	head = tail = curnode = NULL;
	leave_cs(cs);
}

