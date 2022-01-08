#include"pch.h"
#include "ReadHelper.h"
#include "framework.h"
#include <iostream>
#include <fstream>
#include<minwindef.h>
#include<windef.h>
#include<minwinbase.h>
#include<fileapi.h>
#include<handleapi.h>
#include<atlstr.h>
#include "CY_Compress.h"
#include<vector>
#include<io.h>
#include<list>
#include<direct.h>
#include<sstream>
#include<iomanip>


bool  ReadActDzFile(wchar_t* filePath)
{
	//FILE* pFile = fopen("act.dz", "rb");
	CString fileName(filePath);
	FILE* pFile = fopen(fileName, "rb");
	if (pFile == NULL)
	{
		/*AddErrorMessageInFile("添加act.dz文件出错!");*/
		return false;
	}
	DWORD dwSrcBufSize, dwDestLen;
	fread(&dwSrcBufSize, sizeof(DWORD), 1, pFile);
	fread(&dwDestLen, sizeof(DWORD), 1, pFile);
	BYTE* pSrcBuf = new BYTE[dwSrcBufSize];
	BYTE* pDestBuf = new BYTE[dwDestLen];
	fread(pSrcBuf, dwSrcBufSize, 1, pFile);
	int err = uncompress(pDestBuf, &dwDestLen, pSrcBuf, dwSrcBufSize);
	if (err != Z_OK)
	{
		//AddErrorMessageInFile("解压act.dz文件出错!");
		SAFE_DELETE_ARRAY(pSrcBuf);
		SAFE_DELETE_ARRAY(pDestBuf);
		fclose(pFile);
		return false;
	}

	DWORD numAct = 0;
	memcpy(&numAct, pDestBuf, 4);

	DWORD* pOffsetArray = (DWORD*)(pDestBuf + 4);
	for (DWORD index = 0; index < numAct; ++index)
	{
		DWORD s = pOffsetArray[index];
		BYTE* pStart = pDestBuf + pOffsetArray[index]; //该ACT文件文件数据的开始位置
		CY_ACT_INFO* ptempActInfo = new CY_ACT_INFO;
		memcpy(&ptempActInfo->Edition, pStart, sizeof(UINT));
		//if (ptempActInfo->Edition != 60316 && ptempActInfo->Edition != 60306)
		//{
		//	//AddErrorMessageInFile("act.dz文件版本出错!");
		//	//SAFE_DELETE_ARRAY(pSrcBuf);
		//	//SAFE_DELETE_ARRAY(pDestBuf);
		//	//SAFE_DELETE(ptempActInfo);
		//	fclose(pFile);
		//	return false;
		//}
		pStart += sizeof(UINT);
		memcpy(&ptempActInfo->IDname, pStart, sizeof(DWORD64));
		pStart += sizeof(DWORD64);
		memcpy(&ptempActInfo->SecondAct, pStart, sizeof(BYTE));
		pStart += sizeof(BYTE);
		SIZE center = { 0, 0 };
		memcpy(&center, pStart, sizeof(SIZE));
		pStart += sizeof(SIZE);
		ptempActInfo->CenterPoint.x = (SHORT)center.cx;
		ptempActInfo->CenterPoint.y = (SHORT)center.cy;
		memcpy(&ptempActInfo->SumTimeOfAct, pStart, sizeof(LONG));
		pStart += sizeof(LONG);
		memcpy(&ptempActInfo->CountFrameOfAct, pStart, sizeof(BYTE));
		pStart += sizeof(BYTE);
		if (ptempActInfo->CountFrameOfAct != 0)
		{
			ptempActInfo->pAllFrameInfo = new CY_FRAMEINFO[ptempActInfo->CountFrameOfAct];
			for (int i = 0; i < ptempActInfo->CountFrameOfAct; i++)
			{
				CY_FRAMEINFO* tempFrame = &ptempActInfo->pAllFrameInfo[i];
				memcpy(&tempFrame->TimeToNextFrame, pStart, sizeof(LONG));
				pStart += sizeof(LONG);
				memcpy(&tempFrame->CountOfAddEff, pStart, sizeof(BYTE));
				pStart += sizeof(BYTE);
				if (tempFrame->CountOfAddEff != 0)
				{
					tempFrame->pAddEff = new CY_EFFECTINFO[tempFrame->CountOfAddEff];
					for (int i = 0; i < tempFrame->CountOfAddEff; i++)
					{
						memcpy(&tempFrame->pAddEff[i].EffectType, pStart, sizeof(CY_EFFECTTYPE));
						pStart += sizeof(CY_EFFECTTYPE);
						memcpy(&tempFrame->pAddEff[i].EffectData, pStart, sizeof(DWORD64));
						pStart += sizeof(DWORD64);
					}
				}
			}
		}

		//生成act文件
		CString path = "";
		CY_SOURCE_ID sourceID;
		GetSCID(&sourceID, ptempActInfo->IDname);
		path = GetDzPath(sourceID);
		HANDLE hFile;
		DWORD ReadNum = 0;
		hFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			//AddErrorMessageInFile("打开" + path + "文件失败");
			return false;
		}

		WriteFile(hFile, &ptempActInfo->Edition, sizeof(UINT), &ReadNum, NULL);
		WriteFile(hFile, &ptempActInfo->IDname, sizeof(DWORD64), &ReadNum, NULL);

		WriteFile(hFile, &ptempActInfo->SecondAct, sizeof(BYTE), &ReadNum, NULL);
		SIZE center2 = { 0, 0 };
		center2.cx = (LONG)ptempActInfo->CenterPoint.x;
		center2.cy = (LONG)ptempActInfo->CenterPoint.y;

		WriteFile(hFile, &center2, sizeof(SIZE), &ReadNum, NULL);

		WriteFile(hFile, &ptempActInfo->SumTimeOfAct, sizeof(LONG), &ReadNum, NULL);
		WriteFile(hFile, &ptempActInfo->CountFrameOfAct, sizeof(BYTE), &ReadNum, NULL);
		if (ptempActInfo->CountFrameOfAct != 0)
		{
			ptempActInfo->pAllFrameInfo = new CY_FRAMEINFO[ptempActInfo->CountFrameOfAct];
			for (int i = 0; i < ptempActInfo->CountFrameOfAct; i++)
			{
				CY_FRAMEINFO* tempFrame = &ptempActInfo->pAllFrameInfo[i];
				WriteFile(hFile, &tempFrame->TimeToNextFrame, sizeof(LONG), &ReadNum, NULL);
				WriteFile(hFile, &tempFrame->CountOfAddEff, sizeof(BYTE), &ReadNum, NULL);
				if (tempFrame->CountOfAddEff != 0)
				{
					tempFrame->pAddEff = new CY_EFFECTINFO[tempFrame->CountOfAddEff];
					for (int i = 0; i < tempFrame->CountOfAddEff; i++)
					{
						WriteFile(hFile, &tempFrame->pAddEff[i].EffectType, sizeof(CY_EFFECTTYPE), &ReadNum, NULL);
						WriteFile(hFile, &tempFrame->pAddEff[i].EffectData, sizeof(DWORD64), &ReadNum, NULL);
					}
				}
			}
		}

		CloseHandle(hFile);

	}

	SAFE_DELETE_ARRAY(pSrcBuf);
	SAFE_DELETE_ARRAY(pDestBuf);
	fclose(pFile);
	return true;
}

bool  ReadDzFile(wchar_t* filePath)
{
	CY_SOURCE_DATA* pSouData;
	BYTE* pBuf = NULL;
	CString path(filePath);
	int err = UnZipFile(path, &pBuf, NULL);
	//int err = 0;
	if (err != 0)
	{
		return false;
	}
	int FileNum = pBuf[0];
	CY_TGAINFO* pTgaInfoArray = (CY_TGAINFO*)(pBuf + 1);
	int index = 0;
	for (; index < FileNum; index++)
	{
		CString fileName = "";
		CString  actPath = path.Left(path.GetLength() - 3);
		fileName.Format("%s.%03d.tga", actPath, index);
		DWORD dwWidth = 256;
		DWORD dwHeight = 256;
		DWORD offset = pTgaInfoArray[index].offset;
		BYTE * data = pBuf + offset;
		DWORD fileSize = pTgaInfoArray[index].filesize;

		TGAHEAD tgaHead;
		memcpy(&tgaHead, data, sizeof(tgaHead));
		if (tgaHead.Bits != 32)
		{
			return false;
		}
		dwWidth = tgaHead.Width;
		dwHeight = tgaHead.Height;
		DWORD OffBits = sizeof(TGAHEAD) + tgaHead.IdLength;
		DWORD ImageDataSize = dwWidth * dwHeight * tgaHead.Bits / 8;
		BYTE* pData = new BYTE[ImageDataSize];
		if (pData == NULL)
		{
			return false;
		}
		if (tgaHead.ImageType == 2)
		{
			//pData = lpFile+OffBits;
			memcpy(pData, data + OffBits, ImageDataSize);
		}
		else if (tgaHead.ImageType == 10)
		{

			DWORD ComprSize = fileSize - OffBits;
			LPBYTE pTempMem = data + OffBits;
			DWORD posSrc = 0;
			DWORD posDes = 0;
			int BytesPerColor = tgaHead.Bits / 8;
			while ((posSrc < ComprSize) && (posDes < ImageDataSize))
			{
				BYTE tag = pTempMem[posSrc];
				if (tag >= 128)
				{
					int repeats = (int)tag - 127;
					for (int i = 0; i < repeats; i++)
					{
						memcpy(pData + posDes, pTempMem + posSrc + 1, BytesPerColor);
						posDes += BytesPerColor;
					}
					posSrc += BytesPerColor + 1;
				}//end if

				else
				{
					int non_repeats = (int)tag + 1;
					memcpy(pData + posDes, pTempMem + posSrc + 1, non_repeats * BytesPerColor);
					posDes += non_repeats * BytesPerColor;
					posSrc += non_repeats * BytesPerColor + 1;
				}//end else
			}

		}

		FILE* savefile = fopen(fileName, "wb");

		byte type_header[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
		fwrite(type_header, 12 * sizeof(byte), 1, savefile);

		byte header[6];
		header[0] = dwWidth % 256;
		header[1] = dwWidth / 256;
		header[2] = dwHeight % 256;
		header[3] = dwHeight / 256;
		header[4] = 32;
		header[5] = 8;

		fwrite(header, 6 * sizeof(byte), 1, savefile);
		fwrite(pData, ImageDataSize * sizeof(byte), 1, savefile);
		fclose(savefile);
		
	}
	return true;
};

CString  GetDzPath(const CY_SOURCE_ID& sourceID)
{
	CString dzName;
	dzName.Format("%02X%03X%03X%02d%02X.act", sourceID.kind_Id1, sourceID.kind_Id2, sourceID.kind_Id6,
		sourceID.kind_Id3, sourceID.kind_Id4);
	CString Path = "";
	Path.Format("%02X\\%03X\\%03X\\%02d\\", sourceID.kind_Id1, sourceID.kind_Id2, sourceID.kind_Id6,
		sourceID.kind_Id3);
	CString folder = "ACT\\" + Path;

	CreateMultiLevel(folder.GetBuffer(folder.GetLength()));

	Path = "ACT\\" + Path + dzName;

	return Path;
};

DWORD64 SCID2DW64(const CY_SOURCE_ID& s)
{
	DWORD64 temp;
	temp = ((DWORD64)(s.kind_Id1) << 56) + ((DWORD64)(s.kind_Id2) << 40) + ((DWORD64)(s.kind_Id3) << 32)
		+ ((DWORD64)(s.kind_Id4) << 24) + ((DWORD64)(s.kind_Id5) << 16) + (DWORD64)(s.kind_Id6);
	return temp;
}

void GetSCID(CY_SOURCE_ID * s, const DWORD64 id) {

	s->kind_Id1 = (id >> 56) & 0xFF;
	s->kind_Id2 = (id >> 40) & 0xFFFF;
	s->kind_Id3 = (id >> 32) & 0xFF;
	s->kind_Id4 = (id >> 24) & 0xFF;
	s->kind_Id5 = (id >> 16) & 0xFF;
	s->kind_Id6 = id & 0xFFFF;
}

string GetPathDir(string filePath) {
	string dirPath = filePath;
	size_t p = filePath.find_last_of('\\');
	if (p != -1) {
		dirPath.erase(p);
	}
	return dirPath;
};

void CreateMultiLevel(string dir) {
	if (_access(dir.c_str(), 00) == 0) {
		return;
	}
	list<string> dirList;
	dirList.push_front(dir);
	string curDir = GetPathDir(dir);
	while (curDir != dir)
	{
		if (_access(curDir.c_str(), 00) == 0) {
			break;
		}
		dirList.push_front(curDir);
		dir = curDir;
		curDir = GetPathDir(dir);
	}
	for (auto it : dirList)
	{
		_mkdir(it.c_str());
	}
};

