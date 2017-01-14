#pragma once

#include <Windows.h>
class Clipboard
{
public:
	Clipboard();
	~Clipboard();

	void SetClipBoardData(char* StrData);
	void GetClipBoardData(char **DataBuf);


};

