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



//	Desc:	��·��Ϊpath���ļ��������TGA�ļ�ѹ����һ���ļ���
//	Arg:	1��path Ҫѹ�����ļ���  2��destfilename Ŀ���ļ��� 
//			3��pTIArray TGA�ļ���Ϣ��ָ�루����������
//	Return: �����ʶ���������ƣ�����0Ϊ�޴�
int ZipDir( const char* path, const char* destfilename, CY_TGAINFO* pTIArray );

//	Desc:	���ļ���Ϊfilename���ļ���dz��׺����ѹ��һ��BYTE���͵����ݣ�
//			��ppDestBufָ���ָ�루��*ppDestBuf��ָ��������ݣ�
//	Arg:	1��filename Ҫѹ�����ļ���  
//			2��ppDestBuf Ŀ�Ļ���ָ���ָ�룬���ô˺�������*ppDestBuf��Ϊ�գ��û����Լ��ͷŸ�ָ��Ŀռ�
//			3��Ŀ�Ļ���ĳ������ݵ�ָ�룻
//	Return: �����ʶ���������ƣ�����0Ϊ�޴�
int UnZipFile( const char* filename , BYTE** ppDestBuf, DWORD* pDestLen );

#endif
