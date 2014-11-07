/*
名称：llyHttpErrorCode.h
内容：http错误代码
		
制作：卢乐颜
日期：2014.8.21
*/

#ifndef _LLY_HTTP_ERROR_CODE_H_
#define _LLY_HTTP_ERROR_CODE_H_

namespace lly{

//基本错误
#define HTTP_ERROR_OK (200)
#define HTTP_ERROR_EMPTY (204)
#define HTTP_ERROR_CHANGE_SERVER (210)

#define HTTP_ERROR_UNKNOWN (400)			//一般性错误

#define HTTP_ERROR_CONNECT_FAIL (470)
#define HTTP_ERROR_TIMEOUT (471)			//超时
#define HTTP_ERROR_SEND_FAIL (472)
#define HTTP_ERROR_RECV_FAIL (473)
#define HTTP_ERROR_REQUEST_FAIL (474)

#define HTTP_ERROR_OUT_OF_MEMORY (900)

//可自定义错误

const char* getErrorString( int e );

} //lly

#endif //_LLY_HTTP_ERROR_CODE_H_


