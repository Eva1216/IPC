#include "stdafx.h"
#include "NamedPipeClient.h"
#include <iostream>

using namespace std;


CNamedPipeClient::CNamedPipeClient()
{
}


CNamedPipeClient::~CNamedPipeClient()
{
}


void CNamedPipeClient::OpenNamedPipeInClient()
{
    //等待连接命名管道
	WaitNamedPipeA(pPipeName, NMPWAIT_WAIT_FOREVER);
	int a = GetLastError();
  /*  {
        cout<<"命名管道实例不存在 ..."<<endl<<endl;
        return;
    }*/
 
    //打开命名管道
    hNamedPipe = CreateFileA(pPipeName, GENERIC_READ | GENERIC_WRITE, 
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(INVALID_HANDLE_VALUE == hNamedPipe)
    {
        cout<<"打开命名管道失败 ..."<<endl<<endl;
        return;
    }
}
 
 
void CNamedPipeClient::NamedPipeReadInClient()
{
    char				pReadBuf[1000];
	DWORD				dwRead;
	char				BufferData[1000];
    //从命名管道中读取数据
    if(!ReadFile(hNamedPipe, pReadBuf,4  , &dwRead, NULL))
    {
        delete []pReadBuf;
 
        cout<<"读取数据失败 ..."<<endl<<endl;
        return;
    }
}
 
 
void CNamedPipeClient::NamedPipeWriteInClient(char* BufferData)
{
    DWORD                dwWrite;
 
    //向命名管道中写入数据
    if(!WriteFile(hNamedPipe, BufferData, strlen(BufferData), &dwWrite, NULL))
    {
        cout<<"写入数据失败 ..."<<endl<<endl;
        return;
    }
    cout<<"写入数据成功：    "<< BufferData <<endl<<endl;
}