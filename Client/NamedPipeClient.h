#pragma once
#include <Windows.h>
class CNamedPipeClient
{
public:
	CNamedPipeClient();
	~CNamedPipeClient();


	HANDLE            hNamedPipe;

	 
	const char * pPipeName = "\\\\.\\pipe\\yifiPipe";
	//打开命名管道
	void OpenNamedPipeInClient();

	//客户端从命名管道中读取数据
	void NamedPipeReadInClient();

	//客户端往命名管道中写入数据
	void NamedPipeWriteInClient(char* BufferData);
};

