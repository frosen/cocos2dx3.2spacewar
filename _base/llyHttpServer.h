/*
名称：llyHttpServer.h
内容：http式网络通信服务器类
			不适用cocos2dx的构架
			多线程方式
		
制作：卢乐颜
日期：2014.8.21
*/

#ifndef _LLY_HTTP_SERVER_H_
#define _LLY_HTTP_SERVER_H_

#include "cocos2d.h"

namespace lly{

class HttpServer
{
public:
	HttpServer();
	virtual ~HttpServer();

	//创建客户端
	static HttpServer* getInstance();
	static void destoryInstance();

}

} //lly

#endif //_LLY_HTTP_SERVER_H_




