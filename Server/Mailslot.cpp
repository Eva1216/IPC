#include <cstdio>
#include "Mailslot.h"

char* StrRecvData;
CMailslot::CMailslot()
{
	StrRecvData = new char[maxDataLen];
	memset(StrRecvData, 0, maxDataLen);
}


CMailslot::~CMailslot()
{
}

//接收按钮的消息处理
void CMailslot::OnRecv()
{
	DWORD                ReadLength;

	//创建接收数据的线程，将邮槽句柄传递给线程
	CreateRecvThread((LPVOID)MailslotHandle, 0, NULL);
}
//线程处理函数
DWORD WINAPI RecvThreadProc(LPVOID lpPrameter)
{
	HANDLE                hRecvMailSlot;
	DWORD                dwRead;

	hRecvMailSlot = (HANDLE)lpPrameter;

	//利用传进来的邮槽句柄接收收据,并将数据存放到 pStrRecvData 中
	ReadFile(hRecvMailSlot, StrRecvData, maxDataLen, &dwRead, NULL);
	
	int a = GetLastError();

	printf("%s   %d\r\n", StrRecvData,a);
	//关闭邮槽
	CloseHandle(hRecvMailSlot);

	return NULL;
}

void CMailslot::OnCreate()
{
	//创建名为 ZacharyMailSlot 的邮槽
	MailslotHandle = CreateMailslot(TEXT("\\\\.\\mailslot\\YIFIMailSlot"), 0,MAILSLOT_WAIT_FOREVER, NULL);

	if (INVALID_HANDLE_VALUE == this->MailslotHandle)
	{
		MessageBox(NULL,TEXT("创建邮槽失败 ..."), TEXT("提示"), MB_ICONERROR);
		return;
	}
}

HANDLE CMailslot::CreateRecvThread(LPVOID lpParameter, DWORD threadFlag, LPDWORD lpThreadID)
{
	//创建一个线程
	return CreateThread(NULL, 0, RecvThreadProc, lpParameter, threadFlag, lpThreadID);
}