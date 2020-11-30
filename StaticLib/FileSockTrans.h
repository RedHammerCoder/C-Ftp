#pragma once
//#include <windows.h>
#include <string>
#include <memory>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <Windows.h>
#define BUFFERSIZE 1470
#define Offsetof(TYPE, MEMBER) ((int)(&((TYPE *)0)->MEMBER))
extern int BufferPoint;
extern int BufferScaned;
extern char BUFFER[BUFFERSIZE];

using namespace std;



#pragma pack(4)

enum  MsgType:DWORD
{
	SendFile=0,//C 2 S and S 2 C  have file
	RequestSpecFile,
	RequestFileList,//C 2 S FileList
	SendFileNameList,//S 2 C charlist
	Error_Name_Repeat,//client send file and name repeat 
	Error_No_Such_File//Client request a file and no such file in server
};



void Send_ERROR_BACK(SOCKET OppsetSocket, MsgType type);
struct BaseType
{
	MsgType type;
	INT32 LENGTH;
};

struct MEG_SF
{
	MsgType type = SendFile;
	INT32 LENGTH;
	INT32 FileStaticLen;
	WCHAR* FileName;
};



void GetStructType(SOCKET OppsetSocket, char* msg);//receive a charlist and translate to Megtype;

void send_Meg_Sf(SOCKET, const WCHAR* FileName);//TODO:Getting file Handle by file name and send file to Server/Client
HANDLE Receive_Meg_SF( char* msg);//TODO:creatfile and repeat meg,create Handle of file and use recv  WriteFile  

int File_Name_To_MEG_SF(MEG_SF& msg, const WCHAR* FileName);
char* MEG_SF2FlatMem(MEG_SF&);
//---------------------------------------
MEG_SF* FlatMem2Meg_SF(char* meg);
//-----------------------------------------
bool test_MEG_SF_Flat_mem(MEG_SF msg);

struct RequestFile
{
	MsgType type = RequestSpecFile;
	INT32 LENGTH;
	WCHAR* FileName;

};
struct ReqFileList
{
	MsgType type = RequestFileList;
	INT32 LENGTH;
	WCHAR* FileNameList;
};
struct SendFileNameLst
{
	MsgType type = SendFileNameList;
    INT32 LENGTH;
	INT32 FileNameNnmber;
	WCHAR* FileNameList;
		
};
HANDLE Create_Named_File(const WCHAR* Filename);
HANDLE Open_Created_File(const WCHAR* FileName);

void WriteFileDown(HANDLE filehandle,SOCKET OppsetSock);
void Analise_Struct(SOCKET oppsetsocket);

