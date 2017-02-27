#include "stdafx.h"
#include "LPCClient.h"


NTSTATUS(NTAPI* NtConnectPort)(PHANDLE PortHandle, PUNICODE_STRING PortName, PSECURITY_QUALITY_OF_SERVICE SecurityQos, PVOID ClientView, PVOID ServerView, PULONG MaxMessageLength, PVOID ConnectionInformation, PULONG ConnectionInformationLength);
NTSTATUS(NTAPI* RtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
NTSTATUS(NTAPI* NtRequestWaitReplyPort)(HANDLE PortHandle, PPORT_MESSAGE RequestMessage, PPORT_MESSAGE ReplyMessage);
NTSTATUS(NTAPI* NtRequestPort)(HANDLE PortHandle, PPORT_MESSAGE RequestMessage);
NTSTATUS(NTAPI* NtClose)(HANDLE Handle);

CLPCClient::CLPCClient()
{

	m_NtdllModule = GetModuleHandle(L"ntdll.dll");
	if (m_NtdllModule == NULL)
	{
		return;
	}

	(FARPROC&)NtConnectPort = GetProcAddress(m_NtdllModule, "NtConnectPort");
	(FARPROC&)RtlInitUnicodeString = GetProcAddress(m_NtdllModule, "RtlInitUnicodeString");
	(FARPROC&)NtRequestWaitReplyPort = GetProcAddress(m_NtdllModule, "NtRequestWaitReplyPort");
	(FARPROC&)NtRequestPort = GetProcAddress(m_NtdllModule, "NtRequestPort");
	(FARPROC&)NtClose = GetProcAddress(m_NtdllModule, "NtClose");

	m_ClientPortHandle = NULL;

	return;
}


CLPCClient::~CLPCClient()
{
	if (m_ClientPortHandle != NULL)
	{
		NtClose(m_ClientPortHandle);
		m_ClientPortHandle = NULL;
	}
}

BOOL CLPCClient::ConnectServer(WCHAR* ServerName)
{
	SECURITY_QUALITY_OF_SERVICE SecurityService = { 0 };
	NTSTATUS Status = 0;
	UNICODE_STRING uniServerName = { 0 };

	ULONG MaxMessageLength = 0;
	__try
	{
		RtlInitUnicodeString(&uniServerName, ServerName);
		SecurityService.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
		SecurityService.ImpersonationLevel = SecurityImpersonation;
		SecurityService.EffectiveOnly = FALSE;
		SecurityService.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;

		Status = NtConnectPort(&m_ClientPortHandle, &uniServerName, &SecurityService, NULL, NULL, (PULONG)&MaxMessageLength, NULL, NULL);
		int a = GetLastError();
		cout << a << endl;
		if (Status != 0)
		{
			cout << "NtConnectPort Error:" << hex << Status << endl;
			__leave;
		}
	}
	__finally
	{

	}

	return TRUE;
}

BOOL CLPCClient::LPCSendMessage(WCHAR * BufferData, ULONG MsgType)
{
	TRANSFERRED_MESSAGE LPCRequestMessage = { 0 };
	TRANSFERRED_MESSAGE LPCReplayMessage = { 0 };
	NTSTATUS Status = 0;

	InitializeMessageHeader(&LPCRequestMessage.Header, sizeof(TRANSFERRED_MESSAGE), 0);
	InitializeMessageHeader(&LPCReplayMessage.Header, sizeof(TRANSFERRED_MESSAGE), 0);


	wcscpy_s(LPCRequestMessage.MessageText, LPC_MESSAGE_LENGTH, BufferData);
	LPCRequestMessage.Command = MsgType;

	switch (MsgType)
	{
	case LPC_COMMAND_REQUEST_NOREPLY:
		Status = NtRequestPort(m_ClientPortHandle, &LPCRequestMessage.Header);
		break;
	case LPC_COMMAND_REQUEST_REPLY:
		Status = NtRequestWaitReplyPort(m_ClientPortHandle, &LPCRequestMessage.Header, &LPCReplayMessage.Header);
		break;
	case LPC_COMMAND_STOP:
		Status = NtRequestPort(m_ClientPortHandle, &LPCRequestMessage.Header);
		break;
	}

	if (Status != 0)
	{
		cout << "NtRequestPort Error:" << hex << Status << endl;
		return FALSE;
	}

	if (MsgType == LPC_COMMAND_REQUEST_REPLY)
	{
		cout << "Reply Buffer:";
		wcout << LPCReplayMessage.MessageText << endl;
	}


	return TRUE;
}

