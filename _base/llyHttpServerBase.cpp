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
	//1.����������
	if (m_strRootDir == "" || m_nPort == -1) return false;
	
	//2.����������ã�û�оʹ���
	loadSettingFile();
	
	//3.����socket�����
	if (!socket.create(SOCK_STREAM)) return false;
	if (!socket.bind(m_nPort)) return false;
	if (!socket.listen()) return false;

	//4.�����̣߳����տͻ��˵���Ϣ
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

	//��¼���󱨸治�ܹ���
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
	SOCKET s; //�׽���

	while (true)
	{
		//���ܿͻ��˷���������
		if (!socket.accept(s)) 
		{
			recordErrorMsg("accept fail: %d\n",socket.getError());
			break;
		}

		//�������̴߳��������߳��Լ�ִ�У�����Լ�ɾ��
		auto t = std::thread(&HttpServerBase::excuteRequestProcessingThread, this, s);
		t.detach();
	}

	socket.close();
}

void lly::HttpServerBase::excuteRequestProcessingThread( SOCKET s )
{
	lly::HttpSocket socketRequest;

	if (!socketRequest.attach(s)) //socket�����׽���
	{
		recordErrorMsg("attach fail: %d", socketRequest.getError());
		return;
	}

	//�����Ϊ������recv�ĳ�ʱ
	socketRequest.setTimeoutMS(m_nTimeoutMS);

	int nErrorCode = HTTP_ERROR_OK; //�������

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

	//�д������ʹ������
	if (nErrorCode != HTTP_ERROR_OK) 
	{
		auto pAck = new HttpResponse();
		pAck->setErrorCode(nErrorCode);
		socketRequest.sendResponse(pAck);
		delete pAck;
	}

	//�ر�socket
	socketRequest.close();
}



