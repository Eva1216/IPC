// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MailslotClient.h"
#include "NamedPipeClient.h"
int main()
{
	//CMailslotClient v1;
	//v1.OnSend("hello yifi ");
	CNamedPipeClient v1;
	v1.OpenNamedPipeInClient();

	//接收服务端发来的数据
	v1.NamedPipeReadInClient();

	//往命名管道中写入数据
	v1.NamedPipeWriteInClient("Sukang");
	getchar();
	getchar();
    return 0;
}

