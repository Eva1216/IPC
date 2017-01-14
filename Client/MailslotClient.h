#pragma once
#include <Windows.h>
class CMailslotClient
{
public:
	CMailslotClient();
	~CMailslotClient();

	void OnSend(char* szBuffer);
};

