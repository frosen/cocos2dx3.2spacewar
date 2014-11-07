/*
名称：llyHttpServer.h
内容：http式网络通信服务器类
			不适用cocos2dx的构架
			多线程方式
			纯虚函数，需要继承，完成getinstance和处理请求
		
制作：卢乐颜
日期：2014.8.21
*/

#ifndef _LLY_HTTP_SERVER_BASE_H_
#define _LLY_HTTP_SERVER_BASE_H_

#include "llyHttpSocket.h"

#include <memory>
#include <mutex>

namespace lly{

class HttpServerBase
{
public:
#define MAX_LENGTH_OF_ERROR_STRING (10 * 1024)

	HttpServerBase();
	virtual ~HttpServerBase();

	//初始设置
	void setRootDirAndPort(const char* pszRootDir, int nPort);
	void setSettingFilePath(const char* settingFilePath);

	void setTimeoutMS(int nMS) { m_nTimeoutMS = nMS; }

	//开始执行
	bool run();

	//记录错误
	void recordErrorMsg(const char* msg, ...);

protected:
	//开始时读入设置文件里面的属性
	virtual void loadSettingFile() = 0;

	void runHttpInteractionThread();

	void excuteRequestProcessingThread(SOCKET s);

	//处理客户端发来的消息，然后使用sendResponse返回，必须继承
	//返回一个错误码，若不是ok，则发送错误报告给客户端
	virtual void processRequest(
		lly::HttpSocket socketReq, std::shared_ptr<HttpRequest> pReq, int &out_nErrorCode) = 0;

protected:
	lly::HttpSocket socket;
	std::mutex mtx; //线程锁

	std::string m_strRootDir; //数据根目录
	int m_nPort; //端口号
	std::string m_strSettingFilePath;

	//属性
	int m_nTimeoutMS; //超时
	
	std::string m_strErrorRecord; //错误记录
};

} //lly

#endif //_LLY_HTTP_SERVER_BASE_H_




