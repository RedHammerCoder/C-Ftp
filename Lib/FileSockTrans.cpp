#include "FileSockTrans.h"
#include <cassert>
#include <stdlib.h>

//--------------------------
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <MSWSock.h>

 
//-------------------------------
int File_Name_To_MEG_SF(MEG_SF& msg, const WCHAR* FileName)
{
	int FileNameLength = lstrlenW(FileName)+1;

	msg.FileStaticLen = Offsetof(MEG_SF, FileName);
	msg.LENGTH=FileNameLength*sizeof(WCHAR);//LEG==40
	auto temp = (WCHAR*)malloc(msg.LENGTH);
	memcpy(temp, FileName, msg.LENGTH);
	msg.FileName = temp;
	msg.LENGTH += msg.FileStaticLen;
	return msg.LENGTH;
}

char* MEG_SF2FlatMem(MEG_SF &msg)
{
	int temp = (int)msg.LENGTH;
	char* ret = (char*)malloc(temp);
	memcpy(ret, &msg, msg.FileStaticLen);
	int size = (msg.LENGTH - msg.FileStaticLen);
	memcpy(ret + msg.FileStaticLen, msg.FileName,size);
	return ret;
}

void Send_ERROR_BACK(SOCKET OppsetSocket, MsgType type)
{
	

}

void WriteFileDown(HANDLE filehandle, SOCKET OppsetSock)
{
	assert(filehandle != INVALID_HANDLE_VALUE);
	 while (BufferPoint!=0 )
	{
		 while ((BufferPoint - BufferScaned) > 0)
		{
			 DWORD writted=0;
			 WriteFile(filehandle, BUFFER+BufferScaned, BufferPoint - BufferScaned, &writted, NULL);
			//printf("%d bytes is writed to file ",writted);
			BufferScaned += writted;
			if (BufferPoint == BufferScaned)
		 {
			 BufferPoint = 0;
			 BufferScaned = 0;
			 break;
		 }	

		 }
		 if (BufferPoint == BufferScaned)
		 {
			 BufferPoint = 0;
			 BufferScaned = 0;
		 }
		 	 
			 auto temp=recv(OppsetSock, BUFFER  , BUFFERSIZE  ,0);
		     BufferPoint += temp;
			 if (BufferPoint == 0)break;

	

	}
	printf("file writting is finished");
}

void Analise_Struct(SOCKET oppsetsocket)
{
	//init buffer;
	BufferPoint = 0;
	BufferScaned = 0;
	BufferPoint += recv(oppsetsocket, BUFFER, BUFFERSIZE - BufferPoint, 0);
	GetStructType(oppsetsocket, BUFFER);
}




void GetStructType(SOCKET OppsetSocket,  char* msg)//msg指向缓冲区，现在需要缓冲区指针，指向
{//接收对面请求，并进行处理
	BaseType* basetype = new BaseType;
	memcpy(basetype, msg, sizeof(BaseType));
	switch (basetype->type)
	{
	case(SendFile)://收到文件，进行存储
	{
		BufferScaned = 0;
		HANDLE Filehandle = Receive_Meg_SF(msg);//需要更新缓冲区指针
		if (Filehandle == INVALID_HANDLE_VALUE)
		{
			DWORD Err = GetLastError();

		}
		assert(Filehandle != INVALID_HANDLE_VALUE);
		WriteFileDown(Filehandle, OppsetSocket);
	}
		break;
	case(RequestSpecFile)://收到文件请求，进行发送文件
	{
		RequestFile RF;
		memcpy(&RF, msg, basetype->LENGTH);
		send_Meg_Sf(OppsetSocket, RF.FileName);
	}
		break;
	case(RequestFileList)://发送文件列表
		break;
	case(SendFileNameList)://接受文件列表
		break;
	case(Error_Name_Repeat)://client only
		//Send_ERROR_BACK(OppsetSocket, Error_Name_Repeat);
		break;
	case(Error_No_Such_File)://client only
		//Send_ERROR_BACK(OppsetSocket, Error_Name_Repeat);
		break;
	default:
		break;
	}
	closesocket(OppsetSocket);
}
int GetFileLastNameBais(const WCHAR * Path)
{
	
#define CHR_SPLIT L'\\'
#define CHR_END L'\0'
	int Last_Split = 0;
	int poiner = 0;
	while (1)
	{
		if (Path[poiner] == CHR_SPLIT)
			Last_Split = poiner+1;
		if (Path[poiner] == CHR_END)
			break;

		poiner++;
	}
	return Last_Split;
	
}


void send_Meg_Sf(SOCKET OppsetSocket, const WCHAR* FileName)
{
	//openfile 
	HANDLE FileHand = Open_Created_File(FileName);
	MEG_SF heard;
	
	int Length = File_Name_To_MEG_SF(heard, FileName+GetFileLastNameBais(FileName) );
	char* chr= MEG_SF2FlatMem(heard);
	TRANSMIT_FILE_BUFFERS tailer = { 0 };
	tailer.Head = chr;
	tailer.HeadLength = Length;
	auto statu = TransmitFile(OppsetSocket, FileHand , 0, 0, NULL, &tailer, TF_DISCONNECT);
	if (statu == FALSE)
	{
		printf("error at %s,%d", __FILE__, __LINE__);
	}
	


	
}

HANDLE Receive_Meg_SF(char* msg)
{
	
	MEG_SF* fileMsg= FlatMem2Meg_SF(msg);
	BufferScaned += fileMsg->LENGTH;
	HANDLE fileHandle = Create_Named_File(fileMsg->FileName);
	return fileHandle;
	
}

MEG_SF* FlatMem2Meg_SF(char* msg)
{  
	
	MEG_SF* ret = (MEG_SF*)malloc(sizeof(MEG_SF)) ;
	memcpy(ret, msg, Offsetof(MEG_SF, FileName));
	if (ret->type != SendFile)
		exit(1);
	WCHAR* fileName = (WCHAR*)malloc(ret->LENGTH-ret->FileStaticLen);
	ret->FileName = fileName;
	int a = (int)(ret->LENGTH - ret->FileStaticLen);
	char* sourse = msg + ret->FileStaticLen;
	memcpy(fileName,sourse ,a );
	return ret;

}

bool test_MEG_SF_Flat_mem(MEG_SF msg)
{
	wchar_t man;
	WCHAR Name[] = L"helloworld,from 姚聿昕";
	File_Name_To_MEG_SF(msg, (WCHAR*)Name);
	auto chr = MEG_SF2FlatMem(msg);
	MEG_SF* ans = FlatMem2Meg_SF(chr);
	if (ans->type == SendFile && ans->LENGTH == msg.LENGTH && ans->FileStaticLen == msg.FileStaticLen)
	{
		printf("%ls", ans->FileName);
		return true;
	}

}

HANDLE Create_Named_File(const WCHAR* FileName)
{
	HANDLE Fhandle = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (Fhandle == INVALID_HANDLE_VALUE)
	{
		printf("error at %s, %d", __FILE__, __LINE__);
	}
	return Fhandle;
	
}

HANDLE Open_Created_File(const WCHAR* FileName)
{
	HANDLE FHandle = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (FHandle == INVALID_HANDLE_VALUE) {
		printf("error at %s, %d",__FILE__, __LINE__);
	}
	return FHandle;
}
