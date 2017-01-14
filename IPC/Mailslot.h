#pragma once
#include <Windows.h>
//用来定义邮槽发送和接收的最大数据字节数
const int        maxDataLen = 424;
class CMailslot
{
public:

	HANDLE MailslotHandle;
	CMailslot();
	~CMailslot();

	
	void OnRecv();
	void OnCreate();
	 
	//定义一个用来创建线程的成员函数
	HANDLE    CreateRecvThread(LPVOID lpParameter, DWORD threadFlag, LPDWORD lpThreadID);
};

