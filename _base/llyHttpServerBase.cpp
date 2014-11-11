#include "llyHttpServerBase.h"
#include "llyHttpErrorCode.h"
#include <thread>

using namespace lly;

lly::HttpServerBase::HttpServerBase() :
	m_strRootDir(""),
	m_nPort(-1),
	m_nTimeoutMS(3000)
{

}

lly::HttpServerBase::~HttpServerBase()
{
	
}

void lly::HttpServerBase::setRootDirAndPort( const char* pszRootDir, int nPort )
{
	m_strRootDir = pszRootDir;
	m_nPort = nPort;
}

bool lly::HttpServerBase::run()
{
	//1.检测基本设置
	if (m_strRootDir == "" || m_nPort == -1) return false;
	
	//2.检测其他设置，没有就创建
	loadSettingFile();
	
	//3.创建socket服务端
	if (!socket.create(SOCK_STREAM)) return false;
	if (!socket.bind(m_nPort)) return false;
	if (!socket.listen()) return false;

	//4.开启线程，接收客户端的消息
	auto t = std::thread(&HttpServerBase::runHttpInteractionThread, this);
	t.detach();

	return true;
}

void lly::HttpServerBase::recordErrorMsg( const char* msg, ... )
{
	mtx.lock();
	char szErrorMsg[1024];
	va_list	va;
	va_start(va, msg);
	vsprintf(szErrorMsg, msg, va);
	va_end(va);

	//记录错误报告不能过长
	if (m_strErrorRecord.size() > MAX_LENGTH_OF_ERROR_STRING) 
		m_strErrorRecord.clear();

	m_strErrorRecord += szErrorMsg;
	m_strErrorRecord += "\r\n";

	mtx.unlock();
}

void lly::HttpServerBase::clearErrorMsg()
{
	mtx.lock();
	m_strErrorRecord.clear();
	mtx.unlock();
}

void lly::HttpServerBase::runHttpInteractionThread()
{
	SOCKET s; //套接字

	while (true)
	{
		//接受客户端发来的请求
		if (!socket.accept(s)) 
		{
			recordErrorMsg("accept fail: %d\n",socket.getError());
			break;
		}

		//建立新线程处理请求，线程自己执行，最后自己删除
		auto t = std::thread(&HttpServerBase::excuteRequestProcessingThread, this, s);
		t.detach();
	}

	socket.close();
}

void lly::HttpServerBase::excuteRequestProcessingThread( SOCKET s )
{
	lly::HttpSocket socketRequest;

	if (!socketRequest.attach(s)) //socket附着套接字
	{
		recordErrorMsg("attach fail: %d", socketRequest.getError());
		return;
	}

	//这个是为了设置recv的超时
	socketRequest.setTimeoutMS(m_nTimeoutMS);

	int nErrorCode = HTTP_ERROR_OK; //错误代码

	HttpRequest* pReq = nullptr;

	while (true)
	{
		if (pReq) delete pReq;

		pReq = socketRequest.recvRequest();

		if (!pReq)
		{
			recordErrorMsg("receive request fail\n");
			nErrorCode = HTTP_ERROR_REQUEST_FAIL;
			break;
		}

		processRequest(socketRequest, pReq, nErrorCode);

		bool bKeepAlive = pReq->isKeepAlive();

		if (nErrorCode != HTTP_ERROR_OK)
		{
			recordErrorMsg(getErrorString(nErrorCode));
			break;
		}

		if (!bKeepAlive) break;
	}

	if (pReq) delete pReq;

	//有错误则发送错误代码
	if (nErrorCode != HTTP_ERROR_OK) 
	{
		auto pAck = new HttpResponse();
		pAck->setErrorCode(nErrorCode);
		socketRequest.sendResponse(pAck);
		delete pAck;
	}

	//关闭socket
	socketRequest.close();
}



