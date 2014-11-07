#include "llyHttpServer.h"
#include "llyHttpErrorCode.h"

using namespace lly;

lly::HttpServer::HttpServer() :
	m_strRootDir(""),
	m_nPort(-1)
{

}

lly::HttpServer::~HttpServer()
{
	
}

void lly::HttpServer::setRootDirAndPort( const char* pszRootDir, int nPort )
{
	m_strRootDir = pszRootDir;
	m_nPort = nPort;
}

bool lly::HttpServer::run()
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
	auto t = std::thread(&HttpServer::runHttpInteractionThread, this);
	t.detach();

	return true;
}

void lly::HttpServer::recordErrorMsg( const char* msg, ... )
{
	mtx.lock();
	char szErrorMsg[1024];
	va_list	va;
	va_start(va, msg);
	vsprintf(szErrorMsg, msg, va);
	va_end(va);

	//��¼���󱨸治�ܹ���
	if (m_strErrorRecord.size() > 10*1024) m_strErrorRecord.clear();

	m_strErrorRecord += szErrorMsg;
	m_strErrorRecord += "\r\n";

	mtx.unlock();
}

void lly::HttpServer::runHttpInteractionThread()
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
		auto t = std::thread(&HttpServer::excuteRequestProcessingThread, this, s);
		t.detach();

	}

	socket.close();
}

void lly::HttpServer::excuteRequestProcessingThread( SOCKET s )
{
	lly::HttpSocket socketRequest;

	if (!socketRequest.attach(s)) //socket�����׽���
	{
		recordErrorMsg("attach fail: %d", socketRequest.getError());
		return;
	}

	socketRequest.setTimeout(m_nTimeout);
	socketRequest.lly::Socket::setTimeout(m_nTimeout);

	int nErrorCode = HTTP_ERROR_OK; //�������

	while (true)
	{
		auto pReq = socketRequest.recvRequest();

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

	//�д������ʹ������
	if (nErrorCode != HTTP_ERROR_OK) 
	{
		auto pAck = new HttpResponse();
		pAck->setErrorCode(nErrorCode);
		socketRequest.sendResponse(std::shared_ptr<lly::HttpResponse>(pAck));
	}

	//�ر�socket
	socketRequest.close();
}


