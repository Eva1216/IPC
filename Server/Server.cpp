#include "Clipboard.h"
#include <iostream>
#include "NamedPipeServer.h"
#include "Mailslot.h"
#include <Windows.h>
#include "LPCServer.h"
#include "..\\xmlrpc\\src\\XmlRpc.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

void Test_ClipBoard()
{
	Clipboard v1;
	char BufferData[MAX_PATH] = "yifiyifiyifiyifiyifi";
	v1.SetClipBoardData(BufferData);

	char* Buffer = NULL;

	v1.GetClipBoardData(&Buffer);
	printf("%s\r\n", Buffer);
}

void Test_MailSlot()
{
	CMailslot v1;
	v1.OnCreate();
	int a = 0;

	v1.OnRecv();

}

void Test_NamePipe()
{
	CNamedPipeServer v1;
	v1.CreateNamedPipeInServer();
	v1.NamedPipeWriteInServer("YIFI");

	v1.NamedPipeReadInServer();
}
void Test_LPC()
{
	BOOL bIsWow64Process = FALSE;
	if (IsWow64Process(GetCurrentProcess(), &bIsWow64Process) && bIsWow64Process)
	{
		cout << "32位请在32位系统下运行" << endl;
		return ;
	}

	CLPCServer v1;

	v1.StartServer(L"\\LPCServer");
	v1.ListenPort();

	return ;
}

void Test_RPC()
{
	using namespace XmlRpc;

	XmlRpcServer server;
	class Hello : public XmlRpcServerMethod
	{
	public:
		Hello(XmlRpcServer* s) : XmlRpcServerMethod("Hello", s)
		{
		}

		VOID execute(XmlRpcValue& Param, XmlRpcValue& result)
		{
			result = "Hello";
		}

		std::string help()
		{
			return std::string("Say hello");
		}
	}Hello(&server);

	int port = 2356;

	setVerbosity(5);

	server.bindAndListen(port);

	server.enableIntrospection(TRUE);

	server.work(-1.0);
	return;
}
int main()
{
	Test_RPC();
	//Test_LPC();
	//Test_MailSlot();
	getchar();
	getchar();
	int a = 0;
	return 0;
}
