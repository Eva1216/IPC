#pragma once
#include <iostream>
#include <windows.h>
#include <NtSecAPI.h>
#pragma warning(disable:4005)
#include <ntstatus.h>
#pragma warning(default:4005)

#define LPC_MESSAGE_LENGTH 48    //不要超过系统规定

using namespace std;

enum LPC_MSG_TYPE
{
	LPC_COMMAND_REQUEST_NOREPLY,
	LPC_COMMAND_REQUEST_REPLY,
	LPC_COMMAND_STOP
};

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _PORT_MESSAGE
{
	union
	{
		struct
		{
			USHORT DataLength;          // Length of data following the header (bytes)
			USHORT TotalLength;         // Length of data + sizeof(PORT_MESSAGE)
		} s1;
		ULONG Length;
	} u1;

	union
	{
		struct
		{
			USHORT Type;
			USHORT DataInfoOffset;
		} s2;
		ULONG ZeroInit;
	} u2;

	union
	{
		CLIENT_ID ClientId;
		double   DoNotUseThisField;     // Force quadword alignment
	};

	ULONG  MessageId;                   // Identifier of the particular message instance

	union
	{
		ULONG_PTR ClientViewSize;       // Size of section created by the sender (in bytes)
		ULONG  CallbackId;              // 
	};

} PORT_MESSAGE, *PPORT_MESSAGE;

typedef struct _TRANSFERRED_MESSAGE
{
	PORT_MESSAGE Header;

	ULONG   Command;
	WCHAR   MessageText[LPC_MESSAGE_LENGTH];

} TRANSFERRED_MESSAGE, *PTRANSFERRED_MESSAGE;

#define InitializeMessageHeader(ph, l, t)                              \
{                                                                      \
    (ph)->u1.s1.TotalLength      = (USHORT)(l);                        \
    (ph)->u1.s1.DataLength       = (USHORT)(l - sizeof(PORT_MESSAGE)); \
    (ph)->u2.s2.Type             = (USHORT)(t);                        \
    (ph)->u2.s2.DataInfoOffset   = 0;                                  \
    (ph)->ClientId.UniqueProcess = NULL;                               \
    (ph)->ClientId.UniqueThread  = NULL;                               \
    (ph)->MessageId              = 0;                                  \
    (ph)->ClientViewSize         = 0;                                  \
}

class CLPCClient
{
public:
	CLPCClient();
	~CLPCClient();

	BOOL ConnectServer(WCHAR* ServerName);
	BOOL CLPCClient::LPCSendMessage(WCHAR * BufferData, ULONG MsgType);

private:
	HMODULE m_NtdllModule;
	HANDLE  m_ClientPortHandle;
};
