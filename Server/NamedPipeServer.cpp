
#include "NamedPipeServer.h"
#include <iostream>


using namespace std;

CNamedPipeServer::CNamedPipeServer()
{
}


CNamedPipeServer::~CNamedPipeServer()
{
}


void CNamedPipeServer::CreateNamedPipeInServer()
{
	HANDLE                    hEvent;
	OVERLAPPED                ovlpd;

	//首先需要创建命名管道
	//这里创建的是双向模式且使用重叠模式的命名管道
	hNamedPipe = CreateNamedPipeA(pPipeName,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		0, 1, 1024, 1024, 0, NULL);
	cout << "创建命名管道成功 ..." << endl << endl;
	if (INVALID_HANDLE_VALUE == hNamedPipe)
	{
		hNamedPipe = NULL;
		cout << "创建命名管道失败 ..." << endl << endl;
		return;
	}

	//添加事件以等待客户端连接命名管道
	//该事件为手动重置事件，且初始化状态为无信号状态
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hEvent)
	{
		cout << "创建事件失败 ..." << endl << endl;
		return;
	}

	memset(&ovlpd, 0, sizeof(OVERLAPPED));

	//将手动重置事件传递给 ovlap 参数
	ovlpd.hEvent = hEvent;

	//等待客户端连接
	if (!ConnectNamedPipe(hNamedPipe, &ovlpd))
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			CloseHandle(hNamedPipe);
			CloseHandle(hEvent);

			cout << "等待客户端连接失败 ..." << endl << endl;
			return;
		}
	}

	//等待事件 hEvent 失败
	if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE))
	{
		CloseHandle(hNamedPipe);
		CloseHandle(hEvent);

		cout << "等待对象失败 ..." << endl << endl;
		return;
	}

	CloseHandle(hEvent);
}


void CNamedPipeServer::NamedPipeReadInServer()
{
	char *            pReadBuf;
	DWORD            dwRead;

	pReadBuf = new char[strlen(pStr) + 1];
	memset(pReadBuf, 0, strlen(pStr) + 1);

	//从命名管道中读取数据
	if (!ReadFile(hNamedPipe, pReadBuf, strlen(pStr), &dwRead, NULL))
	{
		delete[]pReadBuf;

		cout << "读取数据失败 ..." << endl << endl;
		return;
	}
	cout << "读取数据成功：    " << pReadBuf << endl << endl;
}


void CNamedPipeServer::NamedPipeWriteInServer(char* szBuffer)
{
	DWORD            dwWrite;

	//向命名管道中写入数据
	if (!WriteFile(hNamedPipe, szBuffer, strlen(szBuffer), &dwWrite, NULL))
	{
		cout << "写入数据失败 ..." << endl << endl;
		return;
	}
	cout << "写入数据成功：    " << szBuffer << endl << endl;
}