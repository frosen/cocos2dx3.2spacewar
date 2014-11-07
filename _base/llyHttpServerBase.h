/*
���ƣ�llyHttpServer.h
���ݣ�httpʽ����ͨ�ŷ�������
			������cocos2dx�Ĺ���
			���̷߳�ʽ
			���麯������Ҫ�̳У����getinstance�ʹ�������
		
������¬����
���ڣ�2014.8.21
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

	//��ʼ����
	void setRootDirAndPort(const char* pszRootDir, int nPort);
	void setSettingFilePath(const char* settingFilePath);

	void setTimeoutMS(int nMS) { m_nTimeoutMS = nMS; }

	//��ʼִ��
	bool run();

	//��¼����
	void recordErrorMsg(const char* msg, ...);

protected:
	//��ʼʱ���������ļ����������
	virtual void loadSettingFile() = 0;

	void runHttpInteractionThread();

	void excuteRequestProcessingThread(SOCKET s);

	//����ͻ��˷�������Ϣ��Ȼ��ʹ��sendResponse���أ�����̳�
	//����һ�������룬������ok�����ʹ��󱨸���ͻ���
	virtual void processRequest(
		lly::HttpSocket socketReq, std::shared_ptr<HttpRequest> pReq, int &out_nErrorCode) = 0;

protected:
	lly::HttpSocket socket;
	std::mutex mtx; //�߳���

	std::string m_strRootDir; //���ݸ�Ŀ¼
	int m_nPort; //�˿ں�
	std::string m_strSettingFilePath;

	//����
	int m_nTimeoutMS; //��ʱ
	
	std::string m_strErrorRecord; //�����¼
};

} //lly

#endif //_LLY_HTTP_SERVER_BASE_H_




