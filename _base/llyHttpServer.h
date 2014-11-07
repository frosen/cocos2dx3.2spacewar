/*
���ƣ�llyHttpServer.h
���ݣ�httpʽ����ͨ�ŷ�������
			������cocos2dx�Ĺ���
			���̷߳�ʽ
			���麯������Ҫ�̳У����getinstance�ʹ�������
		
������¬����
���ڣ�2014.8.21
*/

#ifndef _LLY_HTTP_SERVER_H_
#define _LLY_HTTP_SERVER_H_

#include "cocos2d.h"
#include "llyHttpSocket.h"

#include <memory>

namespace lly{

class HttpServer
{
public:
	HttpServer();
	virtual ~HttpServer();

	//��ʼ����
	void setRootDirAndPort(const char* pszRootDir, int nPort);
	void setSettingFilePath(const char* settingFilePath);

	void setTimeout(int nMS) { m_nTimeout = nMS; }

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
	int m_nTimeout; //��ʱ
	
	std::string m_strErrorRecord; //�����¼
};

} //lly

#endif //_LLY_HTTP_SERVER_H_




