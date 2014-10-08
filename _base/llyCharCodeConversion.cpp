#include "llyCharCodeConversion.h"

#ifdef WIN32
#include "win32-specific/icon/include/iconv.h"
#else
#include "iconv.h"
#endif

bool lly::iconvert( const char* from_charset, const char* to_charset, const char* inbuf, int inlen, char* outbuf, int outlen )
{
	iconv_t cd = iconv_open(to_charset, from_charset);

	if (cd == 0) return false;

	const char** pin = &inbuf;

#ifdef WIN32
	const char** pin2 = pin;
#else //linux下iconv的第二个参数不带const
	char** pin2 = const_cast<char**>(pin);
#endif // WIN32

	char** pout = &outbuf;

	memset(outbuf, 0, outlen);

	bool bConv = false;

	bConv = iconv(cd, pin2, (size_t*)&inlen, pout, (size_t*)&outlen) == (size_t)(-1) ? false : true;
	
	iconv_close(cd);

	return bConv;
}

std::string lly::convertGBKToUTF8( const std::string& str )
{
	char textOut[256];

	return lly::iconvert("gb2312", "utf-8", str.c_str(), str.size(),textOut, 256) ? std::string(textOut) : std::string();
}

std::string lly::convertUTF8ToGBK( const std::string& str )
{
	char textOut[256];

	return lly::iconvert("utf-8", "gb2312", str.c_str(), str.size(), textOut, 256) ? std::string(textOut) : std::string();
}

