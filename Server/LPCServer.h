#pragma once

#include <iostream>
#include <windows.h>
#include <NTSecAPI.h>

using namespace std;

#define MAX_LPC_DATA 0x130
#define LPC_MESSAGE_LENGTH  48

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
} CLIENT_ID, *PCLIENT_ID;	//注意此结构体在64 与 32位下的不同

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

} TRANSFERED_MESSAGE, *PTRANSFERED_MESSAGE;

#define InitializeObjectAttributes( p, n, a, r, s ) {   \
    (p)->Length = sizeof( LSA_OBJECT_ATTRIBUTES );      \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }
class CLPCServer
{
public:
	CLPCServer();
	~CLPCServer();
	BOOL StartServer(WCHAR* wzPortName);
	BOOL ListenPort();
private:
	HMODULE m_NtdllModule;
	HANDLE  m_ListenPortHandle;
	HANDLE  m_ClientPortHandle;
};

