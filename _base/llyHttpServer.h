/*
���ƣ�llyHttpServer.h
���ݣ�httpʽ����ͨ�ŷ�������
			������cocos2dx�Ĺ���
			���̷߳�ʽ
		
������¬����
���ڣ�2014.8.21
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

	//�����ͻ���
	static HttpServer* getInstance();
	static void destoryInstance();

}

} //lly

#endif //_LLY_HTTP_SERVER_H_




