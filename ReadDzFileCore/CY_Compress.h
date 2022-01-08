#ifndef CY_COMPRESS
#define CY_COMPRESS

#include "zlib.h"
#include "windows.h"

#pragma comment(lib,"zlib.lib")

struct FileInfo
{
	char filename[24];
	DWORD filesize;
};

struct CY_TGAINFO
{
	DWORD64 srcID;
	DWORD	offset;
	DWORD	filesize;
	SIZE	leftTop;
};



//	Desc:	将路径为path的文件夹里面的TGA文件压缩成一个文件；
//	Arg:	1、path 要压缩的文件夹  2、destfilename 目的文件名 
//			3、pTIArray TGA文件信息的指针（数组名）；
//	Return: 错误标识（并不完善）返回0为无错；
int ZipDir( const char* path, const char* destfilename, CY_TGAINFO* pTIArray );

//	Desc:	将文件名为filename的文件（dz后缀）解压成一段BYTE类型的数据，
//			用ppDestBuf指向的指针（即*ppDestBuf）指向这段数据；
//	Arg:	1、filename 要压缩的文件名  
//			2、ppDestBuf 目的缓冲指针的指针，调用此函数后，若*ppDestBuf不为空，用户须自己释放该指针的空间
//			3、目的缓冲的长度数据的指针；
//	Return: 错误标识（并不完善）返回0为无错；
int UnZipFile( const char* filename , BYTE** ppDestBuf, DWORD* pDestLen );

#endif
