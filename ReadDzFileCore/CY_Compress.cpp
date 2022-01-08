#include "pch.h"
#include "CY_Compress.h"
#include <assert.h>
#include <vector>
#include <algorithm>
#include <functional>

using  namespace std;
//extern void AddErrorMessageInFile(const char* pszString);
#define BUFSIZE 0x100000	//1M
#pragma warning(disable:4996)

//class greater_Cmp: public binary_function<FileInfo*, FileInfo*, bool>
//{
//public:
//	bool operator()(FileInfo* file1, FileInfo* file2)
//	{
//		return  strcmp( file1->filename, file2->filename );
//	}
//};

bool cmp_less(FileInfo* file1, FileInfo* file2)
{
	return  strcmp(file1->filename, file2->filename) < 0;
}

int ZipDir(const char* path, const char* destfilename, CY_TGAINFO* pTIArray)
{
	BOOL bWork = true;
	HANDLE hFile = NULL;
	WIN32_FIND_DATA finder;
	vector<FileInfo*> fileInfoArray;

	char oldpath[200];
	::GetCurrentDirectory(200, oldpath);
	//if (!::SetCurrentDirectory(path))
		//AddErrorMessageInFile("Path name error!");
		//MessageBox( NULL, "Path name error!", NULL, NULL );
	hFile = ::FindFirstFile("*.tga", &finder);
	//FileInfo* pFI = new FileInfo;
	//strcpy( pFI->filename, finder.cFileName );
	//pFI->filesize = finder.nFileSizeLow;
	//fileInfoArray.push_back( pFI);

	while (bWork)
	{
		FileInfo* pFI = new FileInfo;
		strcpy(pFI->filename, finder.cFileName);
		pFI->filesize = finder.nFileSizeLow;
		fileInfoArray.push_back(pFI);
		bWork = ::FindNextFile(hFile, &finder);
	}
	sort(fileInfoArray.begin(), fileInfoArray.end(), cmp_less);

	BYTE* pBuf = new BYTE[BUFSIZE];
	assert(pBuf != NULL);
	DWORD dwOffset = 0;
	pBuf[dwOffset++] = (BYTE)fileInfoArray.size();
	//TgaInfo* pTgaInfoArray = (TgaInfo*)(pBuf + 1);
	dwOffset += pBuf[0] * sizeof(CY_TGAINFO);

	int size = (int)fileInfoArray.size();
	for (int i = 0; i < size; ++i)
	{
		pTIArray[i].offset = dwOffset;
		pTIArray[i].filesize = fileInfoArray[i]->filesize;
		//pTIArray[i].leftTop = pSizeArray[i];
		FILE* pFile = fopen(fileInfoArray[i]->filename, "rb");
		fread(pBuf + dwOffset, fileInfoArray[i]->filesize, 1, pFile);
		dwOffset += fileInfoArray[i]->filesize;
		fclose(pFile);
	}
	memcpy(pBuf + 1, pTIArray, sizeof(CY_TGAINFO) * pBuf[0]);
	//压缩数据
	DWORD dwBufSize = dwOffset;// - pBuf[0] * sizeof(DWORD) * 2 - 1;
	DWORD dwDestLen = dwBufSize;
	BYTE* pBufDest = new BYTE[dwDestLen];
	int err = 0;
	err = compress2(pBufDest, &dwDestLen, pBuf, dwBufSize, Z_BEST_COMPRESSION);

	//写入文件
	//::FindFirstFile( "*.act", &finder );
	char temp[20];
	memcpy(temp, destfilename, strlen(destfilename));
	memcpy(temp + strlen(destfilename), ".dz\0", 4);
	::SetCurrentDirectory("..\\");
	FILE* pFile = fopen(temp, "wb+");
	fwrite(&dwDestLen, sizeof(DWORD), 1, pFile);	//保存目的文件的大小
	fwrite(&dwBufSize, sizeof(DWORD), 1, pFile);	//保存目的文件解压后的大小，即压缩前的pBuf的大小
	fwrite(pBufDest, dwDestLen, 1, pFile);
	fclose(pFile);

	vector<FileInfo*>::iterator ir;
	vector<FileInfo*>::iterator iEnd = fileInfoArray.end();
	for (ir = fileInfoArray.begin(); ir != iEnd; ir++)
	{
		if ((*ir) != NULL)
		{
			delete (*ir);
			(*ir) = NULL;
		}
	}

	if (pBuf != NULL)
	{
		delete[] pBuf;
		pBuf = NULL;
	}

	if (pBufDest != NULL)
	{
		delete[] pBufDest;
		pBufDest = NULL;
	}
	::SetCurrentDirectory(path);
	return err;
}


int UnZipFile(const char* filename, BYTE** ppDestBuf, DWORD* pDestLen)
{
	DWORD dwDestLen;
	if (pDestLen == NULL)
		pDestLen = &dwDestLen;
	FILE * pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		CHAR tempChars[64];
		sprintf(tempChars, "打开dz文件失败%s", filename);
		//AddErrorMessageInFile(tempChars);
		DWORD err = GetLastError();
		assert(0);
		return 1;
	}
	DWORD dwSrcBufSize;
	fread(&dwSrcBufSize, sizeof(DWORD), 1, pFile);
	fread(pDestLen, sizeof(DWORD), 1, pFile);
	BYTE* pSrcBuf = new BYTE[dwSrcBufSize];
	fread(pSrcBuf, dwSrcBufSize, 1, pFile);
	//DWORD aaa = *pDestLen;
	//BYTE* pDestBuf = new BYTE[*pDestLen];
	*ppDestBuf = new BYTE[*pDestLen];
	int err = uncompress(*ppDestBuf, pDestLen, pSrcBuf, dwSrcBufSize);
	if (pSrcBuf != NULL)
	{
		delete[] pSrcBuf;
		pSrcBuf = NULL;
	}
	fclose(pFile);

	return err;
}

//int UnZipFile( const char* filename , BYTE** ppDestBuf, DWORD* pDestLen )
//{
//	DWORD dwDestLen = 0;
//	if(pDestLen == NULL)
//		pDestLen = &dwDestLen;
//	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if(!hFile)
//		return NULL;
//
//	DWORD dwSizeOfFile = GetFileSize(hFile,NULL);
//
//	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwSizeOfFile, NULL);
//	if(!hFileMap)
//		return NULL;
//	LPBYTE lpvoid = (LPBYTE)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
//
//
//	DWORD dwSrcBufSize = 0;
//	//fread( &dwSrcBufSize, sizeof(DWORD), 1, pFile );
//    memcpy( &dwSrcBufSize, lpvoid, 4 );
//	lpvoid += 4;
//	//fread( pDestLen, sizeof(DWORD), 1, pFile );
//    memcpy( pDestLen, lpvoid, 4 );
//    lpvoid += 4;
//	//BYTE* pSrcBuf = new BYTE[dwSrcBufSize];
//	BYTE* pSrcBuf = new BYTE[dwSrcBufSize];
//	//fread( pSrcBuf, dwSrcBufSize, 1, pFile );
//    memcpy( pSrcBuf, lpvoid, dwSrcBufSize );
//
//	*ppDestBuf = new BYTE[*pDestLen];
//	int err = uncompress( *ppDestBuf, pDestLen,  pSrcBuf, dwSrcBufSize );
//
//	UnmapViewOfFile( lpvoid );
//	CloseHandle(hFileMap);
//	CloseHandle(hFile);
//	return err;
//}