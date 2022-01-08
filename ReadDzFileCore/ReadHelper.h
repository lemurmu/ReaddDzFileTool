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
	LONG   lLastUseTime;	//上次使用的时间
	POINT  ptLast;          //上次使用的位置,现在只对地图上纹理作用
	POINTS leftTop;         //该帧图片的左上角（经过裁剪）距离没有经过裁剪图片左上角的位移
	BYTE   poolType;        //标识该纹理是创建在显存还是系统内存
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

//定义元素的资源ID结构
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
//效果类型 
enum CY_EFFECTTYPE
{
	NOTYPE = 0,       //0：没有类型
	ATTACKFRAME,     //1：攻击帧(确定对方被击播放)
	LOOPFRAME,       //2：循环帧(跳)
	LINKFRAME,       //3：连接帧(接下一个动作) 
	STARTFRAME,      //4：起始帧(一个动作的开始计算)
	STARTMOVE,       //5：起步帧(跳) 

	BACKFRAME,       //6：逆序帧(云的播放) 
	FIREFRAME,       //7：射击帧(该帧开始播放射击武器)
	DEFENSEFRAME,    //8：防御帧(虎娃的防御定格)
	LIGHTFRAME,      //9：光效帧(加ID或一个0标识)
	CHANCOLOR,       //10：变色 

	GRAY,            //11：变灰
	CHANSIZE,        //12：变大小
	MUSIC,           //13：音效ID 
	DRAGSHADOW,      //14：拖影
	OFFSETCENTER,    //15：位置偏移

	COLORBRIGHT,     //16：颜色变亮
	COLORDARK,       //17：颜色变暗 
	SWITCHFRAME,     //18：切换帧(战场切换)
	REPEATFRAME,     //19：重复帧(动作播完后重复到哪一帧)
	FLASHFRAME,      //20:闪击帧

	DISAPPEARFRAME,  //21:消失帧
	HEIGHTFIRE,      //22：射击高度
	MOVESPEED,       //23：移动的速度，走一个格子需要的时间（毫秒单位）
};

//加入的效果结构 xwq
struct CY_EFFECTINFO
{
	CY_EFFECTTYPE  EffectType;
	DWORD64        EffectData;
	CY_EFFECTINFO()
	{
		ZeroMemory(this, sizeof(CY_EFFECTINFO));
	}
};

//render时的效果结构 xwq
struct CY_RENDER_EFFECT
{
	CY_EFFECTINFO   ChangeColor;            //变色
	CY_EFFECTINFO   ChangeGray;             //变灰
	CY_EFFECTINFO   ChangeDark;             //变暗
	CY_EFFECTINFO   ChangeBright;           //高亮
	CY_EFFECTINFO   ChangeSize;             //变大小
	CY_EFFECTINFO   OffsetCenter;           //位置偏移
	CY_EFFECTINFO   DragShadow;             //拖影
	CY_EFFECTINFO   Disappear;              //消失
	CY_EFFECTINFO   HeightFire;             //射击高度

	CY_RENDER_EFFECT()
	{
		ZeroMemory(this, sizeof(CY_RENDER_EFFECT));
	}
};

//每帧的结构 xwq
struct CY_FRAMEINFO
{
	LONG            TimeToNextFrame;                  //到下一帧的时间
	BYTE            CountOfAddEff;                    //加入的效果数量
	CY_EFFECTINFO* pAddEff;                          //效果值
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

//act文件的结构 xwq
struct CY_ACT_INFO
{
	UINT        Edition;                                 //版本号     
	DWORD64     IDname;                                  //动作的ID
	BYTE        SecondAct;                               //第2关联动作tga图片序号
	//bool        bBackFrame;                              //是否有逆序帧
	POINTS      CenterPoint;                             //动作图片的基准点
	LONG        SumTimeOfAct;                            //动作的总时间
	BYTE        CountFrameOfAct;                         //动作的帧数
	CY_FRAMEINFO* pAllFrameInfo;                        //动作的帧信息
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