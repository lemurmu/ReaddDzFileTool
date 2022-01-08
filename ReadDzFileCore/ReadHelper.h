#pragma once
#pragma warning(disable:4996)
#include"pch.h"
#include <iostream>
#include <fstream>
#include<minwindef.h>
#include<windef.h>
#include<minwinbase.h>
#include<fileapi.h>
#include<handleapi.h>
#include<atlstr.h>
#include "CY_Compress.h"
#include <vector>
#include<d3d9.h>
using namespace std;

#pragma warning(disable:4996)

#define EXPORT extern "C"  _declspec(dllexport)

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
enum POOLTYPE { NOPOOL = 0, MANAGE = 1, DEFAULT, SYSTEM };


struct CY_SOURCE_DATA
{
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DVERTEXBUFFER9  pVB;
	LONG   lLastUseTime;	//�ϴ�ʹ�õ�ʱ��
	POINT  ptLast;          //�ϴ�ʹ�õ�λ��,����ֻ�Ե�ͼ����������
	POINTS leftTop;         //��֡ͼƬ�����Ͻǣ������ü�������û�о����ü�ͼƬ���Ͻǵ�λ��
	BYTE   poolType;        //��ʶ�������Ǵ������Դ滹��ϵͳ�ڴ�
	CY_SOURCE_DATA()
	{
		pTexture = NULL;
		pVB = NULL;
		lLastUseTime = 0;
		ptLast.x = ptLast.y = 0;
		leftTop.x = leftTop.y = 0;
		poolType = NOPOOL;
	}
	CY_SOURCE_DATA& operator = (const CY_SOURCE_DATA& s)
	{
		this->pTexture = s.pTexture;
		this->pVB = s.pVB;
		this->lLastUseTime = s.lLastUseTime;
		this->leftTop = s.leftTop;
		this->ptLast = s.ptLast;
		this->poolType = s.poolType;
		return *this;
	}
};

struct TGAHEAD
{
	BYTE IdLength;
	BYTE ColorType;
	BYTE ImageType;
	WORD ColorLength;
	BYTE ColorBit;
	WORD X0;
	WORD Y0;
	WORD Width;
	WORD Height;
	BYTE Bits;
	BYTE AttribBit;
};

//����Ԫ�ص���ԴID�ṹ
struct CY_SOURCE_ID
{
	UCHAR kind_Id1;
	WORD  kind_Id2;
	UCHAR kind_Id3;
	UCHAR kind_Id4;
	UCHAR kind_Id5;
	WORD  kind_Id6;
	CY_SOURCE_ID()
	{
		kind_Id1 = 0;
		kind_Id2 = 0;
		kind_Id3 = 0;
		kind_Id4 = 0;
		kind_Id5 = 0;
		kind_Id6 = 0;
	}
	CY_SOURCE_ID(const CY_SOURCE_ID& s)
	{
		this->kind_Id1 = s.kind_Id1;
		this->kind_Id2 = s.kind_Id2;
		this->kind_Id3 = s.kind_Id3;
		this->kind_Id4 = s.kind_Id4;
		this->kind_Id5 = s.kind_Id5;
		this->kind_Id6 = s.kind_Id6;
	}
	CY_SOURCE_ID(const DWORD64& dd)
	{
		this->kind_Id1 = (UCHAR)((dd & 0xFF00000000000000) >> 56);
		this->kind_Id2 = (WORD)((dd & 0xFFFF0000000000) >> 40);
		this->kind_Id3 = (UCHAR)((dd & 0xFF00000000) >> 32);
		this->kind_Id4 = (UCHAR)((dd & 0xFF000000) >> 24);
		this->kind_Id5 = (UCHAR)((dd & 0xFF0000) >> 16);
		this->kind_Id6 = (WORD)(dd & 0xFFFF);
	}
	CY_SOURCE_ID & operator = (const CY_SOURCE_ID & s)
	{
		this->kind_Id1 = s.kind_Id1;
		this->kind_Id2 = s.kind_Id2;
		this->kind_Id3 = s.kind_Id3;
		this->kind_Id4 = s.kind_Id4;
		this->kind_Id5 = s.kind_Id5;
		this->kind_Id6 = s.kind_Id6;
		return *this;
	}
	CY_SOURCE_ID& operator = (const DWORD64 & dd)
	{
		this->kind_Id1 = (UCHAR)((dd & 0xFF00000000000000) >> 56);
		this->kind_Id2 = (WORD)((dd & 0xFFFF0000000000) >> 40);
		this->kind_Id3 = (UCHAR)((dd & 0xFF00000000) >> 32);
		this->kind_Id4 = (UCHAR)((dd & 0xFF000000) >> 24);
		this->kind_Id5 = (UCHAR)((dd & 0xFF0000) >> 16);
		this->kind_Id6 = (WORD)(dd & 0xFFFF);
		return *this;
	}
};
//Ч������ 
enum CY_EFFECTTYPE
{
	NOTYPE = 0,       //0��û������
	ATTACKFRAME,     //1������֡(ȷ���Է���������)
	LOOPFRAME,       //2��ѭ��֡(��)
	LINKFRAME,       //3������֡(����һ������) 
	STARTFRAME,      //4����ʼ֡(һ�������Ŀ�ʼ����)
	STARTMOVE,       //5����֡(��) 

	BACKFRAME,       //6������֡(�ƵĲ���) 
	FIREFRAME,       //7�����֡(��֡��ʼ�����������)
	DEFENSEFRAME,    //8������֡(���޵ķ�������)
	LIGHTFRAME,      //9����Ч֡(��ID��һ��0��ʶ)
	CHANCOLOR,       //10����ɫ 

	GRAY,            //11�����
	CHANSIZE,        //12�����С
	MUSIC,           //13����ЧID 
	DRAGSHADOW,      //14����Ӱ
	OFFSETCENTER,    //15��λ��ƫ��

	COLORBRIGHT,     //16����ɫ����
	COLORDARK,       //17����ɫ�䰵 
	SWITCHFRAME,     //18���л�֡(ս���л�)
	REPEATFRAME,     //19���ظ�֡(����������ظ�����һ֡)
	FLASHFRAME,      //20:����֡

	DISAPPEARFRAME,  //21:��ʧ֡
	HEIGHTFIRE,      //22������߶�
	MOVESPEED,       //23���ƶ����ٶȣ���һ��������Ҫ��ʱ�䣨���뵥λ��
};

//�����Ч���ṹ xwq
struct CY_EFFECTINFO
{
	CY_EFFECTTYPE  EffectType;
	DWORD64        EffectData;
	CY_EFFECTINFO()
	{
		ZeroMemory(this, sizeof(CY_EFFECTINFO));
	}
};

//renderʱ��Ч���ṹ xwq
struct CY_RENDER_EFFECT
{
	CY_EFFECTINFO   ChangeColor;            //��ɫ
	CY_EFFECTINFO   ChangeGray;             //���
	CY_EFFECTINFO   ChangeDark;             //�䰵
	CY_EFFECTINFO   ChangeBright;           //����
	CY_EFFECTINFO   ChangeSize;             //���С
	CY_EFFECTINFO   OffsetCenter;           //λ��ƫ��
	CY_EFFECTINFO   DragShadow;             //��Ӱ
	CY_EFFECTINFO   Disappear;              //��ʧ
	CY_EFFECTINFO   HeightFire;             //����߶�

	CY_RENDER_EFFECT()
	{
		ZeroMemory(this, sizeof(CY_RENDER_EFFECT));
	}
};

//ÿ֡�Ľṹ xwq
struct CY_FRAMEINFO
{
	LONG            TimeToNextFrame;                  //����һ֡��ʱ��
	BYTE            CountOfAddEff;                    //�����Ч������
	CY_EFFECTINFO* pAddEff;                          //Ч��ֵ
	CY_FRAMEINFO()
	{
		ZeroMemory(this, sizeof(CY_FRAMEINFO));
	}
	~CY_FRAMEINFO()
	{
		if (pAddEff != NULL)
		{
			delete[] pAddEff;
			pAddEff = NULL;
		}
	}
};

//act�ļ��Ľṹ xwq
struct CY_ACT_INFO
{
	UINT        Edition;                                 //�汾��     
	DWORD64     IDname;                                  //������ID
	BYTE        SecondAct;                               //��2��������tgaͼƬ���
	//bool        bBackFrame;                              //�Ƿ�������֡
	POINTS      CenterPoint;                             //����ͼƬ�Ļ�׼��
	LONG        SumTimeOfAct;                            //��������ʱ��
	BYTE        CountFrameOfAct;                         //������֡��
	CY_FRAMEINFO* pAllFrameInfo;                        //������֡��Ϣ
	CY_ACT_INFO()
	{
		ZeroMemory(this, sizeof(CY_ACT_INFO));
	}

	~CY_ACT_INFO()
	{
		if (pAllFrameInfo != NULL)
		{
			delete[] pAllFrameInfo;
			pAllFrameInfo = NULL;
		}
	}
};

HANDLE CY_CreateFile(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	return CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
};

EXPORT bool  ReadActDzFile(wchar_t* filePath);

EXPORT bool  ReadDzFile(wchar_t* filePath);

CString  GetDzPath(const CY_SOURCE_ID & sourceID);

DWORD64 SCID2DW64(const CY_SOURCE_ID & s);

void GetSCID(CY_SOURCE_ID * s, const DWORD64 id);

string GetPathDir(string filePath);

void CreateMultiLevel(string dir);