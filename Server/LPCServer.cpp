
#include "LPCServer.h"

NTSTATUS(NTAPI* NtCreatePort)(PHANDLE PortHandle, PLSA_OBJECT_ATTRIBUTES ObjectAttributes, ULONG MaxConnectionInfoLength, ULONG MaxMessageLength, PULONG MaxPoolUsage);
NTSTATUS(NTAPI* NtAcceptConnectPort)(PHANDLE PortHandle, PVOID PortContext, PPORT_MESSAGE ConnectionRequest, BOOL AcceptConnection, PVOID ServerView, PVOID ClientView);
NTSTATUS(NTAPI* NtCompleteConnectPort)(HANDLE PortHandle);
NTSTATUS(NTAPI* NtReplyWaitReceivePort)(HANDLE PortHandle, PVOID* PortContext, PPORT_MESSAGE ReplyMessage, PPORT_MESSAGE ReceiveMessage);
NTSTATUS(NTAPI* RtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
NTSTATUS(NTAPI* NtListenPort)(HANDLE PortHandle, PPORT_MESSAGE ConnectionRequest);
NTSTATUS(NTAPI* NtReplyPort)(HANDLE PortHandle, PPORT_MESSAGE ReplyMessage);
NTSTATUS(NTAPI* NtClose)(HANDLE Handle);

CLPCServer::CLPCServer()
{
	m_NtdllModule = GetModuleHandle(L"ntdll.dll");
	if (m_NtdllModule == NULL)
	{
		return;
	}

	(FARPROC&)NtCreatePort = GetProcAddress(m_NtdllModule, "NtCreatePort");
	(FARPROC&)NtAcceptConnectPort = GetProcAddress(m_NtdllModule, "NtAcceptConnectPort");
	(FARPROC&)NtCompleteConnectPort = GetProcAddress(m_NtdllModule, "NtCompleteConnectPort");
	(FARPROC&)NtReplyWaitReceivePort = GetProcAddress(m_NtdllModule, "NtReplyWaitReceivePort");
	(FARPROC&)RtlInitUnicodeString = GetProcAddress(m_NtdllModule, "RtlInitUnicodeString");
	(FARPROC&)NtListenPort = GetProcAddress(m_NtdllModule, "NtListenPort");
	(FARPROC&)NtReplyPort = GetProcAddress(m_NtdllModule, "NtReplyPort");
	(FARPROC&)NtClose = GetProcAddress(m_NtdllModule, "NtClose");

	m_ClientPortHandle = NULL;
	m_ListenPortHandle = NULL;
}


//

CLPCServer::~CLPCServer()
{
	if (m_ClientPortHandle != NULL)
	{
		NtClose(m_ClientPortHandle);
	}

	if (m_ListenPortHandle != NULL)
	{
		NtClose(m_ListenPortHandle);
	}
}

BOOL CLPCServer::StartServer(WCHAR*  PortName)
{
	SECURITY_DESCRIPTOR   SecurityDescriptor = { 0 };
	UNICODE_STRING		  uniPortName;
	LSA_OBJECT_ATTRIBUTES ObjectAttribute = { 0 };
	NTSTATUS			  Status = 0;

	__try
	{
		if (!InitializeSecurityDescriptor(&SecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
		{
			cout << "InitializeSecurityDescriptor Error:" << GetLastError() << endl;
			__leave;
		}

		if (!SetSecurityDescriptorDacl(&SecurityDescriptor, TRUE, NULL, FALSE))
		{
			cout << "SetSecurityDescriptorDacl Error:" << GetLastError() << endl;
			__leave;
		}

		// 创建端口
		RtlInitUnicodeString(&uniPortName, PortName);
		InitializeObjectAttributes(&ObjectAttribute, &uniPortName, 0, NULL, &SecurityDescriptor);
		// 短消息直接传送 - 长消息 View传送
		Status = NtCreatePort(&m_ListenPortHandle, &ObjectAttribute, NULL, sizeof(PORT_MESSAGE) + MAX_LPC_DATA, 0);
		if (Status != 0)
		{
			cout << "NtCreatePort Error:" << GetLastError() << endl;
			__leave;
		}
	}
	__finally
	{

	}

	return TRUE;
}

BOOL CLPCServer::ListenPort()
{
	NTSTATUS	Status = 0;
	CHAR BufferData[sizeof(PORT_MESSAGE) + MAX_LPC_DATA] = { 0 };
	PTRANSFERED_MESSAGE LPCRequestMessage = (PTRANSFERED_MESSAGE)BufferData;
	TRANSFERED_MESSAGE  LPCReplyMessage = { 0 };


	// 监听端口
	Status = NtListenPort(m_ListenPortHandle, &LPCRequestMessage->Header);
	if (Status != 0)
	{
		cout << "NtListenPort Error:" << GetLastError() << endl;

		return FALSE;;
	}
	// 得到连接请求
	Status = NtAcceptConnectPort(&m_ClientPortHandle, NULL, &LPCRequestMessage->Header, TRUE, NULL, NULL);
	if (Status < 0)
	{
		cout << "NtAcceptConnectPort Error:" << GetLastError() << endl;
		NtClose(m_ListenPortHandle);

		return FALSE;
	}

	// 完成
	Status = NtCompleteConnectPort(m_ClientPortHandle);   //回传数据
	if (Status < 0)
	{
		cout << "NtCompleteConnectPort Error:" << GetLastError() << endl;
		NtClose(m_ListenPortHandle);
		NtClose(m_ClientPortHandle);
		return FALSE;
	}

	cout << endl;
	cout << "ProcessID:" << dec << (ULONG_PTR)LPCRequestMessage->Header.ClientId.UniqueProcess << " MessageType:" << LPCRequestMessage->Header.u2.s2.Type
		<< " MessageSize:" << LPCRequestMessage->Header.u1.s1.TotalLength << endl;


	BOOL bOk = TRUE;
	while (bOk)
	{
		// 等待传送消息 - 单线程限制？
		Status = NtReplyWaitReceivePort(m_ClientPortHandle, NULL, NULL, &LPCReplyMessage.Header);
		if (Status < 0)
		{
			cout << "NtReplyWaitReceivePort Error:" << GetLastError() << endl;
			break;
		}
		switch (LPCReplyMessage.Command)
		{
		case LPC_COMMAND_REQUEST_NOREPLY:
		{
			cout << "Receive ClientID:" << LPCReplyMessage.Header.ClientId.UniqueProcess << " Message:" << "MessageType:" << LPCReplyMessage.Command << endl;
			wcout << L"Buffer:" << LPCReplyMessage.MessageText << endl;   //客户端无视
			break;
		}
		case LPC_COMMAND_REQUEST_REPLY:
		{
			cout << "Receive ClientID:" << LPCReplyMessage.Header.ClientId.UniqueProcess << " Message:" << "MessageType:" << LPCReplyMessage.Command << endl;
			wcout << L"Buffer:" << LPCReplyMessage.MessageText << endl;
			Status = NtReplyPort(m_ClientPortHandle, &LPCReplyMessage.Header);   //确认收到回传客户端
			if (Status < 0)
			{
				cout << "NtReplyPort Error:" << hex << Status << endl;

				bOk = FALSE;

				break;
			}
			break;
		}
		case LPC_COMMAND_STOP:
		{
			bOk = FALSE;

			break;
		}
		}
	}

	if (m_ClientPortHandle != NULL)
	{
		NtClose(m_ClientPortHandle);
	}

	if (m_ListenPortHandle != NULL)
	{
		NtClose(m_ListenPortHandle);
	}
}

