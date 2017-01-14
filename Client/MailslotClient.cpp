#include "stdafx.h"
#include "MailslotClient.h"


CMailslotClient::CMailslotClient()
{
}


CMailslotClient::~CMailslotClient()
{
}

void CMailslotClient::OnSend(char* szBuffer)
{
	HANDLE				SendMailSlotHandle;
	DWORD				dwWrite;

	//打开由服务端创建的邮件槽
	SendMailSlotHandle = CreateFile(TEXT("\\\\.\\mailslot\\YIFIMailSlot"),
		GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == SendMailSlotHandle)
	{
		MessageBox(NULL,TEXT("打开邮槽失败 ..."), TEXT("提示"), MB_ICONERROR);
		return;
	}
	//通过邮件槽向服务端发送数据
	if (!WriteFile(SendMailSlotHandle, szBuffer, strlen(szBuffer), &dwWrite, NULL))
	{
		MessageBox(NULL,TEXT("写入数据失败 ..."), TEXT("提示"), MB_ICONERROR);

		CloseHandle(SendMailSlotHandle);
		return;
	}
	MessageBox(NULL,TEXT("写入数据成功 ..."), TEXT("提示"), MB_ICONINFORMATION);

 
	 
}
