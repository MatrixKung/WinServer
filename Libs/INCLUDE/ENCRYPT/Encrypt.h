#ifndef __ENCRYPT__
#define __ENCRYPT__

#include "BlowFish.h"
#include "Rijndael.h"
#include "XOR256.h"
#include "Mars.h"
#include "serpent.h"

const int BlockSize = 16;			//���������ﶨ��Կ����Ϊ128bit

class CEncrypt
{
public:
	enum EncryptEnum
	{
		Encrypt_XOR256,				//����ģʽ
		Encrypt_BlowFish_ECB,		//BlowFish��ECBģʽ
		Encrypt_BlowFish_CBC,		//BlowFish��CBCģʽ
		Encrypt_BlowFish_CFB,		//BlowFish��CFBģʽ
		Encrypt_AES_ECB,			//AES��ECBģʽ
		Encrypt_AES_CBC,			//AES��CBCģʽ
		Encrypt_AES_CFB,			//AES��CFBģʽ
		Encrypt_MARS,				//IBM's MARS�����㷨
		Encrypt_SERPENT,			//Serpent�㷨
		Encrypt_MaxNum
	};
	bool MakeKey(EncryptEnum type, const char* KeyValue);
	bool Encrypt(char* InString, char* OutString, int BufferLen);
	bool Decrypt(char* InString, char* OutString, int BufferLen);
private:
	EncryptEnum m_type;
	char PublicKey[BlockSize+1];	//��Կ
	char XORKey[257];				//���256�ֽ���Կ					
};

#endif