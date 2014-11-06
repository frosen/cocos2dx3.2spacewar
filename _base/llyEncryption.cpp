#include <string.h>
#include <stdio.h>
#include "llyEncryption.h"  

using namespace lly;


#define F_(x,y, z) ((x & y) | (~x & z))
#define G_(x,y,z) ((x & z) | (y & ~z))
#define H_(x,y,z) (x^y^z)
#define I_(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac) \
{ \
	a += F_(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define GG(a,b,c,d,x,s,ac) \
{ \
	a += G_(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define HH(a,b,c,d,x,s,ac) \
{ \
	a += H_(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define II(a,b,c,d,x,s,ac) \
{ \
	a += I_(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#if LLY_MD5 == LLY_enable

unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

MD5::MD5_CTX lly::MD5::md5 = {0};

void lly::MD5::encodeString( char* input_string, char out_md5[33], NUM_OF_BIT num )
{
	MD5Init(&md5);	
	lly::MD5::MD5Update(&md5, (unsigned char*)input_string, strlen((char *)input_string));

	unsigned char result[16];
	lly::MD5::MD5Final(&md5, result);

	//把生成的unsigned char 转换成字符串
	int index;
	int nsize;

	if (num == NUM_OF_BIT::_32)
	{
		index = 0;
		nsize = 16;
	}
	else
	{
		index = 4;
		nsize = 12;
	}

	for(; index < nsize; ++index)  
	{  
		sprintf(out_md5 + index * 2, "%02x" , result[index]);
	}  
	
	if (num == NUM_OF_BIT::_32) out_md5[32] = '\0';
	else out_md5[16] = '\0';
}

void lly::MD5::encodeFile( char* input_filename, char out_md5[33], NUM_OF_BIT num )
{

}

void lly::MD5::MD5Init(MD5_CTX *context)  
{  
	context->count[0] = 0;  
	context->count[1] = 0;  
	context->state[0] = 0x67452301;  
	context->state[1] = 0xEFCDAB89;  
	context->state[2] = 0x98BADCFE;  
	context->state[3] = 0x10325476;  
}

void lly::MD5::MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)  
{  
	unsigned int i = 0,index = 0,partlen = 0;  
	index = (context->count[0] >> 3) & 0x3F;  
	partlen = 64 - index;  
	context->count[0] += inputlen << 3;  
	if(context->count[0] < (inputlen << 3))  
		context->count[1]++;  
	context->count[1] += inputlen >> 29;  

	if(inputlen >= partlen)  
	{  
		memcpy(&context->buffer[index],input,partlen);  
		MD5Transform(context->state,context->buffer);  
		for(i = partlen;i+64 <= inputlen;i+=64)  
			MD5Transform(context->state,&input[i]);  
		index = 0;          
	}    
	else  
	{  
		i = 0;  
	}  
	memcpy(&context->buffer[index],&input[i],inputlen-i);  
}

void lly::MD5::MD5Final(MD5_CTX *context,unsigned char digest[16])  
{  
	unsigned int index = 0,padlen = 0;  
	unsigned char bits[8];  
	index = (context->count[0] >> 3) & 0x3F;  
	padlen = (index < 56)?(56-index):(120-index);  
	MD5Encode(bits,context->count,8);  
	MD5Update(context,PADDING,padlen);  
	MD5Update(context,bits,8);  
	MD5Encode(digest,context->state,16);  
}

void lly::MD5::MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)  
{  
	unsigned int i = 0,j = 0;  
	while(j < len)  
	{  
		output[j] = input[i] & 0xFF;    
		output[j+1] = (input[i] >> 8) & 0xFF;  
		output[j+2] = (input[i] >> 16) & 0xFF;  
		output[j+3] = (input[i] >> 24) & 0xFF;  
		i++;  
		j+=4;  
	}  
}

void lly::MD5::MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)  
{  
	unsigned int i = 0,j = 0;  
	while(j < len)  
	{  
		output[i] = (input[j]) |  
			(input[j+1] << 8) |  
			(input[j+2] << 16) |  
			(input[j+3] << 24);  
		i++;  
		j+=4;   
	}  
}

void lly::MD5::MD5Transform(unsigned int state[4],unsigned char block[64])  
{  
	unsigned int a = state[0];  
	unsigned int b = state[1];  
	unsigned int c = state[2];  
	unsigned int d = state[3];  
	unsigned int x[64];  
	MD5Decode(x,block,64);  
	FF(a, b, c, d, x[ 0], 7, 0xd76aa478);   
	FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);   
	FF(c, d, a, b, x[ 2], 17, 0x242070db);   
	FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);   
	FF(a, b, c, d, x[ 4], 7, 0xf57c0faf);   
	FF(d, a, b, c, x[ 5], 12, 0x4787c62a);   
	FF(c, d, a, b, x[ 6], 17, 0xa8304613);   
	FF(b, c, d, a, x[ 7], 22, 0xfd469501);   
	FF(a, b, c, d, x[ 8], 7, 0x698098d8);   
	FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);   
	FF(c, d, a, b, x[10], 17, 0xffff5bb1);   
	FF(b, c, d, a, x[11], 22, 0x895cd7be);   
	FF(a, b, c, d, x[12], 7, 0x6b901122);   
	FF(d, a, b, c, x[13], 12, 0xfd987193);   
	FF(c, d, a, b, x[14], 17, 0xa679438e);   
	FF(b, c, d, a, x[15], 22, 0x49b40821);   


	GG(a, b, c, d, x[ 1], 5, 0xf61e2562);   
	GG(d, a, b, c, x[ 6], 9, 0xc040b340);   
	GG(c, d, a, b, x[11], 14, 0x265e5a51);   
	GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);   
	GG(a, b, c, d, x[ 5], 5, 0xd62f105d);   
	GG(d, a, b, c, x[10], 9,  0x2441453);   
	GG(c, d, a, b, x[15], 14, 0xd8a1e681);   
	GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);   
	GG(a, b, c, d, x[ 9], 5, 0x21e1cde6);   
	GG(d, a, b, c, x[14], 9, 0xc33707d6);   
	GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);   
	GG(b, c, d, a, x[ 8], 20, 0x455a14ed);   
	GG(a, b, c, d, x[13], 5, 0xa9e3e905);   
	GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8);   
	GG(c, d, a, b, x[ 7], 14, 0x676f02d9);   
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);   


	HH(a, b, c, d, x[ 5], 4, 0xfffa3942);   
	HH(d, a, b, c, x[ 8], 11, 0x8771f681);   
	HH(c, d, a, b, x[11], 16, 0x6d9d6122);   
	HH(b, c, d, a, x[14], 23, 0xfde5380c);   
	HH(a, b, c, d, x[ 1], 4, 0xa4beea44);   
	HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);   
	HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);   
	HH(b, c, d, a, x[10], 23, 0xbebfbc70);   
	HH(a, b, c, d, x[13], 4, 0x289b7ec6);   
	HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);   
	HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);   
	HH(b, c, d, a, x[ 6], 23,  0x4881d05);   
	HH(a, b, c, d, x[ 9], 4, 0xd9d4d039);   
	HH(d, a, b, c, x[12], 11, 0xe6db99e5);   
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8);   
	HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);   


	II(a, b, c, d, x[ 0], 6, 0xf4292244);   
	II(d, a, b, c, x[ 7], 10, 0x432aff97);   
	II(c, d, a, b, x[14], 15, 0xab9423a7);   
	II(b, c, d, a, x[ 5], 21, 0xfc93a039);   
	II(a, b, c, d, x[12], 6, 0x655b59c3);   
	II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);   
	II(c, d, a, b, x[10], 15, 0xffeff47d);   
	II(b, c, d, a, x[ 1], 21, 0x85845dd1);   
	II(a, b, c, d, x[ 8], 6, 0x6fa87e4f);   
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0);   
	II(c, d, a, b, x[ 6], 15, 0xa3014314);   
	II(b, c, d, a, x[13], 21, 0x4e0811a1);   
	II(a, b, c, d, x[ 4], 6, 0xf7537e82);   
	II(d, a, b, c, x[11], 10, 0xbd3af235);   
	II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);   
	II(b, c, d, a, x[ 9], 21, 0xeb86d391);   
	state[0] += a;  
	state[1] += b;  
	state[2] += c;  
	state[3] += d;  
}

#endif
//==============================================
#if LLY_SHA1 == LLY_enable

#define GET_UINT32(n,b,i)               \
{                               \
	(n) = ( (uint32) (b)[(i)   ] << 24 )     \
	| ( (uint32) (b)[(i) + 1] << 16 )     \
	| ( (uint32) (b)[(i) + 2] << 8 )     \
	| ( (uint32) (b)[(i) + 3]     );     \
}

#define PUT_UINT32(n,b,i)               \
{                               \
	(b)[(i)   ] = (uint8) ( (n) >> 24 );     \
	(b)[(i) + 1] = (uint8) ( (n) >> 16 );     \
	(b)[(i) + 2] = (uint8) ( (n) >> 8 );     \
	(b)[(i) + 3] = (uint8) ( (n)     );     \
}

void lly::Sha1::encodeString( char* input_string, char out_sha1[41] )
{
	unsigned char sha1sum[20];
	sha1_context ctx;

	sha1_starts( &ctx );
	sha1_update( &ctx, (uint8 *) input_string, strlen( input_string ) );
	sha1_finish( &ctx, sha1sum );

	for( int j = 0; j < 20; j++ )
	{
		sprintf( out_sha1 + j * 2, "%02x", sha1sum[j] );
	}

	out_sha1[40] = '\0';
}

void lly::Sha1::encodeFile( char* input_filename, char out_sha1[41] )
{

}

void lly::Sha1::sha1_starts( sha1_context *ctx )
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;

	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xC3D2E1F0;
}

void lly::Sha1::sha1_process( sha1_context *ctx, uint8 data[64] )
{
	uint32 temp, W[16], A, B, C, D, E;

	GET_UINT32( W[0], data, 0 );
	GET_UINT32( W[1], data, 4 );
	GET_UINT32( W[2], data, 8 );
	GET_UINT32( W[3], data, 12 );
	GET_UINT32( W[4], data, 16 );
	GET_UINT32( W[5], data, 20 );
	GET_UINT32( W[6], data, 24 );
	GET_UINT32( W[7], data, 28 );
	GET_UINT32( W[8], data, 32 );
	GET_UINT32( W[9], data, 36 );
	GET_UINT32( W[10], data, 40 );
	GET_UINT32( W[11], data, 44 );
	GET_UINT32( W[12], data, 48 );
	GET_UINT32( W[13], data, 52 );
	GET_UINT32( W[14], data, 56 );
	GET_UINT32( W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                             \
	(                                     \
	temp = W[(t - 3) & 0x0F] ^ W[(t - 8) & 0x0F] ^   \
	W[(t - 14) & 0x0F] ^ W[ t     & 0x0F],     \
	( W[t & 0x0F] = S(temp,1) )                 \
	)

#define P(a,b,c,d,e,x)                       \
	{                                     \
	e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);     \
	}

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

	P( A, B, C, D, E, W[0] );
	P( E, A, B, C, D, W[1] );
	P( D, E, A, B, C, W[2] );
	P( C, D, E, A, B, W[3] );
	P( B, C, D, E, A, W[4] );
	P( A, B, C, D, E, W[5] );
	P( E, A, B, C, D, W[6] );
	P( D, E, A, B, C, W[7] );
	P( C, D, E, A, B, W[8] );
	P( B, C, D, E, A, W[9] );
	P( A, B, C, D, E, W[10] );
	P( E, A, B, C, D, W[11] );
	P( D, E, A, B, C, W[12] );
	P( C, D, E, A, B, W[13] );
	P( B, C, D, E, A, W[14] );
	P( A, B, C, D, E, W[15] );
	P( E, A, B, C, D, R(16) );
	P( D, E, A, B, C, R(17) );
	P( C, D, E, A, B, R(18) );
	P( B, C, D, E, A, R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

	P( A, B, C, D, E, R(20) );
	P( E, A, B, C, D, R(21) );
	P( D, E, A, B, C, R(22) );
	P( C, D, E, A, B, R(23) );
	P( B, C, D, E, A, R(24) );
	P( A, B, C, D, E, R(25) );
	P( E, A, B, C, D, R(26) );
	P( D, E, A, B, C, R(27) );
	P( C, D, E, A, B, R(28) );
	P( B, C, D, E, A, R(29) );
	P( A, B, C, D, E, R(30) );
	P( E, A, B, C, D, R(31) );
	P( D, E, A, B, C, R(32) );
	P( C, D, E, A, B, R(33) );
	P( B, C, D, E, A, R(34) );
	P( A, B, C, D, E, R(35) );
	P( E, A, B, C, D, R(36) );
	P( D, E, A, B, C, R(37) );
	P( C, D, E, A, B, R(38) );
	P( B, C, D, E, A, R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

	P( A, B, C, D, E, R(40) );
	P( E, A, B, C, D, R(41) );
	P( D, E, A, B, C, R(42) );
	P( C, D, E, A, B, R(43) );
	P( B, C, D, E, A, R(44) );
	P( A, B, C, D, E, R(45) );
	P( E, A, B, C, D, R(46) );
	P( D, E, A, B, C, R(47) );
	P( C, D, E, A, B, R(48) );
	P( B, C, D, E, A, R(49) );
	P( A, B, C, D, E, R(50) );
	P( E, A, B, C, D, R(51) );
	P( D, E, A, B, C, R(52) );
	P( C, D, E, A, B, R(53) );
	P( B, C, D, E, A, R(54) );
	P( A, B, C, D, E, R(55) );
	P( E, A, B, C, D, R(56) );
	P( D, E, A, B, C, R(57) );
	P( C, D, E, A, B, R(58) );
	P( B, C, D, E, A, R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

	P( A, B, C, D, E, R(60) );
	P( E, A, B, C, D, R(61) );
	P( D, E, A, B, C, R(62) );
	P( C, D, E, A, B, R(63) );
	P( B, C, D, E, A, R(64) );
	P( A, B, C, D, E, R(65) );
	P( E, A, B, C, D, R(66) );
	P( D, E, A, B, C, R(67) );
	P( C, D, E, A, B, R(68) );
	P( B, C, D, E, A, R(69) );
	P( A, B, C, D, E, R(70) );
	P( E, A, B, C, D, R(71) );
	P( D, E, A, B, C, R(72) );
	P( C, D, E, A, B, R(73) );
	P( B, C, D, E, A, R(74) );
	P( A, B, C, D, E, R(75) );
	P( E, A, B, C, D, R(76) );
	P( D, E, A, B, C, R(77) );
	P( C, D, E, A, B, R(78) );
	P( B, C, D, E, A, R(79) );

#undef K
#undef F

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
	ctx->state[4] += E;
}

void lly::Sha1::sha1_update( sha1_context *ctx, uint8 *input, uint32 length )
{
	uint32 left, fill;

	if( ! length ) return;

	left = ctx->total[0] & 0x3F;
	fill = 64 - left;

	ctx->total[0] += length;
	ctx->total[0] &= 0xFFFFFFFF;

	if( ctx->total[0] < length )
		ctx->total[1]++;

	if( left && length >= fill )
	{
		memcpy( (void *) (ctx->buffer + left),
			(void *) input, fill );
		sha1_process( ctx, ctx->buffer );
		length -= fill;
		input += fill;
		left = 0;
	}

	while( length >= 64 )
	{
		sha1_process( ctx, input );
		length -= 64;
		input += 64;
	}

	if( length )
	{
		memcpy( (void *) (ctx->buffer + left),
			(void *) input, length );
	}
}

static uint8 sha1_padding[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void lly::Sha1::sha1_finish( sha1_context *ctx, uint8 digest[20] )
{
	uint32 last, padn;
	uint32 high, low;
	uint8 msglen[8];

	high = ( ctx->total[0] >> 29 )
		| ( ctx->total[1] << 3 );
	low = ( ctx->total[0] << 3 );

	PUT_UINT32( high, msglen, 0 );
	PUT_UINT32( low, msglen, 4 );

	last = ctx->total[0] & 0x3F;
	padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

	sha1_update( ctx, sha1_padding, padn );
	sha1_update( ctx, msglen, 8 );

	PUT_UINT32( ctx->state[0], digest, 0 );
	PUT_UINT32( ctx->state[1], digest, 4 );
	PUT_UINT32( ctx->state[2], digest, 8 );
	PUT_UINT32( ctx->state[3], digest, 12 );
	PUT_UINT32( ctx->state[4], digest, 16 );
}

#endif
//==============================================
#if LLY_CHECK_SUM == LLY_enable

unsigned int lly::CheckSum::GetCheckSum32(const char *pData,int len)
{
	if (len==0) len=strlen(pData);

	unsigned int nRet=0;
	const unsigned char *s=(const unsigned char *)pData;
	int cnShift=0;
	for (int i=0;i<len;i++)
	{
		nRet=nRet+(((unsigned long long)(*s++))<<cnShift);
		cnShift+=8;
		if (cnShift>=32) cnShift=0;
	}
	return nRet;
}

unsigned long long lly::CheckSum::GetCheckSum64(const char *pData,int len)
{
	if (len==0) len=strlen(pData);

	unsigned long long nRet=0;
	const unsigned char *s=(const unsigned char *)pData;
	int cnShift=0;
	for (int i=0;i<len;i++)
	{
		nRet=nRet+(((unsigned long long)(*s++))<<cnShift);
		cnShift+=8;
		if (cnShift>=64) cnShift=0;
	}
	return nRet;
}

void lly::CheckSum::Uint64ToBase16(unsigned long long nValue,char *pBase16)
{
	for (int i=0;i<16;i++)
	{
		pBase16[i]=(nValue & 0xf)+'a';
		nValue>>=4;
	}
	pBase16[16]=0;
}

void lly::CheckSum::GetCheckSumStr16(const char *pSrc,int len,char *pDst)
{
	unsigned long long llCheckSum=GetCheckSum64(pSrc,len);
	Uint64ToBase16(llCheckSum,pDst);
}

#endif

//============================================
#if LLY_BASE64 == LLY_enable

const static unsigned int MAX_LINE_LENGTH = 76;  

const static char BASE64_ALPHABET [64] =   
{  
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', //   0 -   9  
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', //  10 -  19  
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', //  20 -  29  
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //  30 -  39  
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', //  40 -  49  
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', //  50 -  59  
	'8', '9', '+', '/'                //  60 -  63  
};  

const static char BASE64_DEALPHABET [128] =   
{  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //   0 -   9  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  10 -  19  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  20 -  29  
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  30 -  39  
	0,  0,  0, 62,  0,  0,  0, 63, 52, 53, //  40 -  49  
	54, 55, 56, 57, 58, 59, 60, 61,  0,  0, //  50 -  59  
	0, 61,  0,  0,  0,  0,  1,  2,  3,  4, //  60 -  69  
	5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //  70 -  79  
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, //  80 -  89  
	25,  0,  0,  0,  0,  0,  0, 26, 27, 28, //  90 -  99  
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // 100 - 109  
	39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 110 - 119  
	49, 50, 51,  0,  0,  0,  0,  0      // 120 - 127  
}; 

void lly::Base64::encodeString( const char* input_string, std::string &out_base64 )
{
	char* pOutputBuffer;  

	CreateMatchingEncodingBuffer(strlen(input_string), &pOutputBuffer);
	EncodeBuffer(const_cast<char*>(input_string), strlen(input_string), pOutputBuffer);

	out_base64 = std::string(pOutputBuffer);

	free(pOutputBuffer);
}

void lly::Base64::dncodeString( const char* input_base64, std::string &out_string )
{
	char* pDecode;
	CreateMatchingDecodingBuffer(const_cast<char*>(input_base64), &pDecode);
	DecodeBuffer(const_cast<char*>(input_base64), pDecode);

	out_string = std::string(pDecode);

	free(pDecode);
}

unsigned int Base64::encodeFile (const char* SourceFileName, const char* EncodedFileName)  
{  
	// 	CFile InputFile;  
	// 	CFile OutputFile;  
	// 
	// 	if (!InputFile.Open (p_pSourceFileName, CFile::modeRead))  
	// 		return UNABLE_TO_OPEN_INPUT_FILE;  
	// 
	// 	if (!OutputFile.Open (p_pEncodedFileName, CFile::modeCreate|CFile::modeWrite))  
	// 		return UNABLE_TO_OPEN_OUTPUT_FILE;  
	// 
	// 	char InputBuffer [19 * 3];  
	// 	char* pOutputBuffer;  
	// 	CreateMatchingEncodingBuffer (sizeof (InputBuffer), &pOutputBuffer);  
	// 
	// 	if (pOutputBuffer == 0)  
	// 		return UNABLE_TO_CREATE_OUTPUTBUFFER;  
	// 
	// 	unsigned int ReadBytes = 0;  
	// 	while ((ReadBytes = InputFile.Read (InputBuffer, sizeof (InputBuffer))) != 0)  
	// 	{  
	// 		EncodeBuffer (InputBuffer, ReadBytes, pOutputBuffer);  
	// 		OutputFile.Write (pOutputBuffer, strlen (pOutputBuffer));  
	// 	}  
	// 
	// 	OutputFile.Flush ();  
	// 	OutputFile.Close ();  
	// 	InputFile.Close  ();  

	return 0;  
}  

unsigned int Base64::decodeFile (const char* SourceFileName, const char* DecodedFileName)  
{  
	// 	CStdioFile  InputFile;  
	// 	CFile       OutputFile;  
	// 
	// 	if (!InputFile.Open (p_pSourceFileName, CFile::modeRead))  
	// 		return UNABLE_TO_OPEN_INPUT_FILE;  
	// 
	// 	if (!OutputFile.Open (p_pDecodedFileName, CFile::modeCreate|CFile::modeWrite))  
	// 		return UNABLE_TO_OPEN_OUTPUT_FILE;  
	// 
	// 	CString InputBuffer;  
	// 	char    OutputBuffer[64];  
	// 
	// 	unsigned int ReadBytes = 0;  
	// 	while ((ReadBytes = InputFile.ReadString (InputBuffer)) != 0)  
	// 	{  
	// 		InputBuffer.Remove ('\r');  
	// 		InputBuffer.Remove ('\n');  
	// 		unsigned int DecodedBytes = DecodeBuffer ((LPTSTR) (LPCTSTR) InputBuffer, OutputBuffer);  
	// 		OutputFile.Write (&OutputBuffer [0], DecodedBytes);  
	// 	}  
	// 
	// 	OutputFile.Flush ();  
	// 	OutputFile.Close ();  
	// 	InputFile.Close  ();  

	return 0;  
}

unsigned int Base64::CalculateRecquiredEncodeOutputBufferSize (unsigned int p_InputByteCount)  
{  
	div_t result = div (p_InputByteCount, 3);  

	unsigned int RecquiredBytes = 0;  
	if (result.rem == 0)  
	{  
		// Number of encoded characters  
		RecquiredBytes = result.quot * 4;  

		// CRLF -> "\r\n" each 76 characters  
		result = div (RecquiredBytes, 76);  
		RecquiredBytes += result.quot * 2;  

		// Terminating null for the Encoded String  
		RecquiredBytes += 1;  

		return RecquiredBytes;  
	}  
	else  
	{  
		// Number of encoded characters  
		RecquiredBytes = result.quot * 4 + 4;  

		// CRLF -> "\r\n" each 76 characters  
		result = div (RecquiredBytes, 76);  
		RecquiredBytes += result.quot * 2;  

		// Terminating null for the Encoded String  
		RecquiredBytes += 1;  

		return RecquiredBytes;  
	}  
}  

unsigned int Base64::CalculateRecquiredDecodeOutputBufferSize (char* p_pInputBufferString)  
{  
	unsigned int BufferLength = strlen (p_pInputBufferString);  

	div_t result = div (BufferLength, 4);  

	if (p_pInputBufferString [BufferLength - 1] != '=')  
	{  
		return result.quot * 3;  
	}  
	else  
	{  
		if (p_pInputBufferString [BufferLength - 2] == '=')  
		{  
			return result.quot * 3 - 2;  
		}  
		else  
		{  
			return result.quot * 3 - 1;  
		}  
	}  
}  

void Base64::EncodeByteTriple (char* p_pInputBuffer, unsigned int InputCharacters, char* p_pOutputBuffer)  
{  
	unsigned int mask = 0xfc000000;  
	unsigned int buffer = 0;  


	char* temp = (char*) &buffer;  
	temp [3] = p_pInputBuffer [0];  
	if (InputCharacters > 1)  
		temp [2] = p_pInputBuffer [1];  
	if (InputCharacters > 2)  
		temp [1] = p_pInputBuffer [2];  

	switch (InputCharacters)  
	{  
	case 3:  
		{  
			p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			buffer = buffer << 6;  
			p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			buffer = buffer << 6;  
			p_pOutputBuffer [2] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			buffer = buffer << 6;  
			p_pOutputBuffer [3] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			break;  
		}  
	case 2:  
		{  
			p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			buffer = buffer << 6;  
			p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			buffer = buffer << 6;  
			p_pOutputBuffer [2] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			p_pOutputBuffer [3] = '=';  
			break;  
		}  
	case 1:  
		{  
			p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			buffer = buffer << 6;  
			p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];  
			p_pOutputBuffer [2] = '=';  
			p_pOutputBuffer [3] = '=';  
			break;  
		}  
	}  
}  

unsigned int Base64::DecodeByteQuartet (char* p_pInputBuffer, char* p_pOutputBuffer)  
{  
	unsigned int buffer = 0;  

	if (p_pInputBuffer[3] == '=')  
	{  
		if (p_pInputBuffer[2] == '=')  
		{  
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;  
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;  
			buffer = buffer << 14;  

			char* temp = (char*) &buffer;  
			p_pOutputBuffer [0] = temp [3];  

			return 1;  
		}  
		else  
		{  
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;  
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;  
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[2]]) << 6;  
			buffer = buffer << 8;  

			char* temp = (char*) &buffer;  
			p_pOutputBuffer [0] = temp [3];  
			p_pOutputBuffer [1] = temp [2];  

			return 2;  
		}  
	}  
	else  
	{  
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;  
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;  
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[2]]) << 6;  
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[3]]) << 6;   
		buffer = buffer << 2;  

		char* temp = (char*) &buffer;  
		p_pOutputBuffer [0] = temp [3];  
		p_pOutputBuffer [1] = temp [2];  
		p_pOutputBuffer [2] = temp [1];  

		return 3;  
	}  

	return -1;  
}  

void Base64::EncodeBuffer(char* p_pInputBuffer, unsigned int p_InputBufferLength, char* p_pOutputBufferString)  
{  
	unsigned int FinishedByteQuartetsPerLine = 0;  
	unsigned int InputBufferIndex  = 0;  
	unsigned int OutputBufferIndex = 0;  

	memset (p_pOutputBufferString, 0, CalculateRecquiredEncodeOutputBufferSize (p_InputBufferLength));  

	while (InputBufferIndex < p_InputBufferLength)  
	{  
		if (p_InputBufferLength - InputBufferIndex <= 2)  
		{  
			FinishedByteQuartetsPerLine ++;  
			EncodeByteTriple (p_pInputBuffer + InputBufferIndex, p_InputBufferLength - InputBufferIndex, p_pOutputBufferString + OutputBufferIndex);  
			break;  
		}  
		else  
		{  
			FinishedByteQuartetsPerLine++;  
			EncodeByteTriple (p_pInputBuffer + InputBufferIndex, 3, p_pOutputBufferString + OutputBufferIndex);  
			InputBufferIndex  += 3;  
			OutputBufferIndex += 4;  
		}  

		if (FinishedByteQuartetsPerLine == 19)  
		{  
			p_pOutputBufferString [OutputBufferIndex  ] = '\r';  
			p_pOutputBufferString [OutputBufferIndex+1] = '\n';  
			p_pOutputBufferString += 2;  
			FinishedByteQuartetsPerLine = 0;  
		}  
	}  
}  

unsigned int Base64::DecodeBuffer (char* p_pInputBufferString, char* p_pOutputBuffer)  
{  
	unsigned int InputBufferIndex  = 0;  
	unsigned int OutputBufferIndex = 0;  
	unsigned int InputBufferLength = strlen (p_pInputBufferString);  

	char ByteQuartet [4];  

	while (InputBufferIndex < InputBufferLength)  
	{  
		for (int i = 0; i < 4; i++)  
		{  
			ByteQuartet [i] = p_pInputBufferString [InputBufferIndex];  

			// Ignore all characters except the ones in BASE64_ALPHABET  
			if ((ByteQuartet [i] >= 48 && ByteQuartet [i] <=  57) ||  
				(ByteQuartet [i] >= 65 && ByteQuartet [i] <=  90) ||  
				(ByteQuartet [i] >= 97 && ByteQuartet [i] <= 122) ||  
				ByteQuartet [i] == '+' || ByteQuartet [i] == '/' || ByteQuartet [i] == '=')  
			{  
			}  
			else  
			{  
				// Invalid character  
				i--;  
			}  

			InputBufferIndex++;  
		}  

		OutputBufferIndex += DecodeByteQuartet (ByteQuartet, p_pOutputBuffer + OutputBufferIndex);  
	}  

	// OutputBufferIndex gives us the next position of the next decoded character  
	// inside our output buffer and thus represents the number of decoded characters  
	// in our buffer.  
	return OutputBufferIndex;  
}  

unsigned int Base64::CreateMatchingEncodingBuffer (unsigned int p_InputByteCount, char** p_ppEncodingBuffer)  
{  
	unsigned int Size = CalculateRecquiredEncodeOutputBufferSize (p_InputByteCount);  
	(*p_ppEncodingBuffer) = (char*) malloc (Size);  
	memset (*p_ppEncodingBuffer, 0, Size);  
	return Size;  
}  

unsigned int Base64::CreateMatchingDecodingBuffer (char* p_pInputBufferString, char** p_ppDecodingBuffer)  
{  
	unsigned int Size = CalculateRecquiredDecodeOutputBufferSize (p_pInputBufferString);  
	(*p_ppDecodingBuffer) = (char*) malloc (Size+1);  
	memset (*p_ppDecodingBuffer, 0, Size+1);  
	return Size+1;  
}  

#endif




