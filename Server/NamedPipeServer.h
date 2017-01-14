#pragma once
#pragma once

#include <Windows.h>
class CNamedPipeServer
{
public:
	CNamedPipeServer();
	~CNamedPipeServer();

	HANDLE            hNamedPipe;
	const char *    pStr = "yifi";
	const char *    pPipeName = "\\\\.\\pipe\\yifiPipe";
	void CNamedPipeServer::NamedPipeReadInServer();
	void CNamedPipeServer::NamedPipeWriteInServer(char* szBuffer);
	void CNamedPipeServer::CreateNamedPipeInServer();


};