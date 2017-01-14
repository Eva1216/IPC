#include "stdafx.h"
#include "Clipboard.h"


Clipboard::Clipboard()
{
}


Clipboard::~Clipboard()
{
}


void Clipboard::SetClipBoardData(char* StrData)
{
	//将 OpenClipboard 函数的参数指定为 NULL，表明为当前进程打开剪贴板
	if (OpenClipboard(NULL))
	{
		char * pDataBuf;

		//全局内存对象
		HGLOBAL hGlobalClip;

		//给全局内存对象分配全局内存
		hGlobalClip = GlobalAlloc(GHND, strlen(StrData) + 1);
		//通过给全局内存对象加锁获得对全局内存块的引用
		pDataBuf = (char *)GlobalLock(hGlobalClip);
		memcpy(pDataBuf, StrData, sizeof(StrData));
		//使用完全局内存块后需要对全局内存块解锁
		GlobalUnlock(hGlobalClip);

		//清空剪贴板
		EmptyClipboard();
		//设置剪贴板数据，这里直接将数据放到了剪贴板中，而没有使用延迟提交技术
		SetClipboardData(CF_TEXT, hGlobalClip);
		//关闭剪贴板
		CloseClipboard();
	}
}

void Clipboard::GetClipBoardData(char **DataBuf)
{
	if (OpenClipboard(NULL))
	{
		
		//判断剪贴板中的数据格式是否为 CF_TEXT
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			
			HGLOBAL            hGlobalClip;

			//从剪贴板中获取格式为 CF_TEXT 的数据
			hGlobalClip = GetClipboardData(CF_TEXT);
			GlobalLock(hGlobalClip);
			 
			//memcpy(DataBuf, szBuffer, sizeof(szBuffer));
			*DataBuf = (char *)GlobalLock(hGlobalClip);
			GlobalUnlock(hGlobalClip);
		}
		CloseClipboard();
	}
}