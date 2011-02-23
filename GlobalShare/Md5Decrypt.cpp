#include "typedef.h"
#include "Md5Decrypt.h"

Md5Decrypt::Md5Decrypt()
{
	Li=&M[0];
	Ri=&M[32];
	KL=&K[0];
	KR=&K[28];	
}

/// 接口函数：加密数据
int Md5Decrypt::DesEncrypt( char *Out, char *In, long datalen, const char *Key, int keylen)
{
	
	if (Des_Go(Out, In, datalen, Key, keylen, ENCRYPT)==TRUE) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/// 接口函数：解密数据
int Md5Decrypt::DesDecrypt( char *Out, char *In, long datalen, const char *Key, int keylen)
{
	if (Des_Go(Out, In, datalen, Key, keylen, DECRYPT)==TRUE) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//////////////////////////////////////////////////////////////////////////

bool Md5Decrypt::Des_Go(char *Out, char *In, long datalen, const char *Key, int keylen, bool Type)
{
	if( !( Out && In && Key && (datalen=(datalen+7)&0xfffffff8) ) ) 
		return false;
	SetKey(Key, keylen);
	if( !Is3DES ) {   // 1次DES
		for(long i=0,j=datalen>>3; i<j; ++i,Out+=8,In+=8)
			DES(Out, In, &SubKey[0], Type);
	} else{   // 3次DES 加密:加(key0)-解(key1)-加(key0) 解密::解(key0)-加(key1)-解(key0)
		for(long i=0,j=datalen>>3; i<j; ++i,Out+=8,In+=8) {
			DES(Out, In,  &SubKey[0], Type);
			DES(Out, Out, &SubKey[1], !Type);
			DES(Out, Out, &SubKey[0], Type);
		}
	}
	return true;
}

void Md5Decrypt::SetKey(const char* Key, int len)
{
	memset(deskey, 0, 16);
	memcpy(deskey, Key, len>16?16:len);
	SetSubKey(&SubKey[0], &deskey[0]);
	Is3DES = len>8 ? (SetSubKey(&SubKey[1], &deskey[8]), true) : false;
}

void Md5Decrypt::DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type)
{
	
	ByteToBit(M, In, 64);
	Transform(M, M, IP_Table, 64);
	if( Type == ENCRYPT ){
		for(int i=0; i<16; ++i) {
			memcpy(tmp, Ri, 32);
			F_func(Ri, (*pSubKey)[i]);
			Xor(Ri, Li, 32);
			memcpy(Li, tmp, 32);
		}
	}else{
		for(int i=15; i>=0; --i) {
			memcpy(tmp, Li, 32);
			F_func(Li, (*pSubKey)[i]);
			Xor(Li, Ri, 32);
			memcpy(Ri, tmp, 32);
		}
	}
	Transform(M, M, IPR_Table, 64);
	BitToByte(Out, M, 64);
}

void Md5Decrypt::SetSubKey(PSubKey pSubKey, const char Key[8])
{
	ByteToBit(K, Key, 64);
	Transform(K, K, PC1_Table, 56);
	for(int i=0; i<16; ++i) {
		RotateL(KL, 28, LOOP_Table[i]);
		RotateL(KR, 28, LOOP_Table[i]);
		Transform((*pSubKey)[i], K, PC2_Table, 48);
	}
}

void Md5Decrypt::F_func(bool In[32], const bool Ki[48])
{
	
	Transform(MR, In, E_Table, 48);
	Xor(MR, Ki, 48);
	S_func(In, MR);
	Transform(In, In, P_Table, 32);
}

void Md5Decrypt::S_func(bool Out[32], const bool In[48])
{
	for(char i=0,j,k; i<8; ++i,In+=6,Out+=4) {
		j = (char)( (In[0]<<1) + In[5] );
		k = (char)( (In[1]<<3) + (In[2]<<2) + (In[3]<<1) + In[4] );
		ByteToBit(Out, &S_Box[i][j][k], 4);
	}
}

void Md5Decrypt::Transform(bool *Out, bool *In, const char *Table, int len)
{
	for(int i=0; i<len; ++i)
		Tmp[i] = In[ Table[i]-1 ];
	memcpy(Out, Tmp, len);
}

void Md5Decrypt::Xor(bool *InA, const bool *InB, int len)
{
	for(int i=0; i<len; ++i)
		InA[i] ^= InB[i];
}

void Md5Decrypt::RotateL(bool *In, int len, int loop)
{
	memcpy(Tmp, In, loop);
	memcpy(In, In+loop, len-loop);
	memcpy(In+len-loop, Tmp, loop);
}

void Md5Decrypt::ByteToBit(bool *Out, const char *In, int bits)
{
	for(int i=0; i<bits; ++i)
		Out[i] = (In[i>>3]>>(i&7)) & 1;
}

void Md5Decrypt::BitToByte(char *Out, const bool *In, int bits)
{
	memset(Out, 0, bits>>3);
	for(int i=0; i<bits; ++i)
		Out[i>>3] |= In[i]<<(i&7);
}

