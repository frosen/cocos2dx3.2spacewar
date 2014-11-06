/*
	���ƣ�llyEncryption.h
	���������ּ�����
	���ߣ�����
	���ڣ�2014.10.30
*/

#ifndef _LLY_ENCRYPTION_H_
#define _LLY_ENCRYPTION_H_

#include <stdlib.h>  
#include <math.h>  
#include <memory.h>
#include <string>

//ʹ���ĸ���define��enable״̬
#define LLY_enable 1
#define LLY_disable 0

#define LLY_MD5 (LLY_enable)
#define LLY_SHA1 (LLY_enable)
#define LLY_CHECK_SUM (LLY_enable)
#define LLY_BASE64 (LLY_enable)

namespace lly{

#if LLY_MD5 == LLY_enable

class MD5
{
public:
	enum class NUM_OF_BIT
	{
		_32,
		_16,
	};

	struct MD5_CTX //md5��������content
	{  
		unsigned int count[2];  
		unsigned int state[4];  
		unsigned char buffer[64];
	};

	//��ȡ�ַ�����md5���룬
	//���һ��16���ַ���32λ����ɵ�MD5�ַ�����β������һ��0����һ��33λ��,
	//����ѡ����32λ���뻹��16λ
	//ע������С���������
	static void encodeString(
		char* input_string, 
		char out_md5[33], 
		NUM_OF_BIT num = NUM_OF_BIT::_32);

	//��ȡ�ļ���md5���룬
	//�����ļ�ȫ·�������һ��16���ַ���32λ����ɵ�MD5�ַ�����β������һ��0����һ��33λ��
	//����ѡ����32λ���뻹��16λ
	//ע������С���������
	static void encodeFile(
		char* input_filename, 
		char out_md5[33],
		NUM_OF_BIT num = NUM_OF_BIT::_32);

private:
	static void MD5Init(MD5_CTX *context);  
	static void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);  
	static void MD5Final(MD5_CTX *context,unsigned char digest[16]);  
	static void MD5Transform(unsigned int state[4],unsigned char block[64]);  
	static void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);  
	static void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);

private:
	static MD5_CTX md5;
};

#endif

#if LLY_SHA1 == LLY_enable

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif
class Sha1
{
public:
	struct sha1_context//sha1��������
	{
		uint32 total[2];
		uint32 state[5];
		uint8 buffer[64];
	};

	//��ȡ�ַ�����sha1���룬
	//���һ��20���ַ���40λ����ɵ�MD5�ַ�����β������һ��0����һ��41λ��,
	//ע������С���������
	static void encodeString(	char* input_string, char out_sha1[41]);

	//��ȡ�ļ���sha1���룬
	//���һ��20���ַ���40λ����ɵ�MD5�ַ�����β������һ��0����һ��41λ��,
	//ע������С���������
	static void encodeFile(char* input_filename, char out_sha1[41]);
	
private:
	static void sha1_starts( sha1_context *ctx );
	static void sha1_update( sha1_context *ctx, uint8 *input, uint32 length );
	static void sha1_finish( sha1_context *ctx, uint8 digest[20] );

private:
	static void sha1_process( sha1_context *ctx, uint8 data[64] );
};

#endif

#if LLY_CHECK_SUM == LLY_enable

class CheckSum
{
public:
	//��ȡһ���ַ�����У����ַ���
	static void GetCheckSumStr16(const char *pSrc,int len,char *pDst16);

	//����ܺͼ�����
	static unsigned int GetCheckSum32(const char *pData,int len=0);
	static unsigned long long GetCheckSum64(const char *pData,int len=0);

	//��һ��64λ�޷�������ת��Ϊ16�ֽڵ��ַ���������ĸa��ʼ��16����ĸ
	static void Uint64ToBase16(unsigned long long nValue,char *pBase16);
};

#endif

#if LLY_BASE64 == LLY_enable

class Base64  
{  
public:
	//����ͽ���һ���ַ��������һ��base64���ַ�������ֹcharй¶�ڴ棩
	static void encodeString(const char* input_string, std::string &out_base64);
	static void dncodeString(const char* input_base64, std::string &out_string);

	//����ͽ���һ���ļ������һ��base64���ļ�
	static unsigned int encodeFile (const char* SourceFileName, const char* EncodedFileName);  
	static unsigned int decodeFile (const char* SourceFileName, const char* DecodedFileName);

private:
	static unsigned int CalculateRecquiredEncodeOutputBufferSize (unsigned int p_InputByteCount);
	static unsigned int CalculateRecquiredDecodeOutputBufferSize (char* p_pInputBufferString);

	static void EncodeByteTriple  (char* p_pInputBuffer, unsigned int InputCharacters, char* p_pOutputBuffer);  
	static unsigned int DecodeByteQuartet (char* p_pInputBuffer, char* p_pOutputBuffer);  

	static void EncodeBuffer (char* p_pInputBuffer, unsigned int p_InputBufferLength, char*p_pOutputBufferString);  
	static unsigned int DecodeBuffer (char* p_pInputBufferString, char* p_pOutputBuffer);  

	static unsigned int CreateMatchingEncodingBuffer (unsigned int p_InputByteCount, char** p_ppEncodingBuffer);  
	static unsigned int CreateMatchingDecodingBuffer (char* p_pInputBufferString, char** p_ppDecodingBuffer);    
};

#endif

} //lly



#endif //_LLY_ENCRYPTION_H_



