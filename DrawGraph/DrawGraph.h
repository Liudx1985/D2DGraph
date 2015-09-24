
// DrawGraph.h : DrawGraph 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CDrawGraphApp:
// 有关此类的实现，请参阅 DrawGraph.cpp
//
class CDrawGraphView;
class CDrawGraphApp : public CWinAppEx
{
public:
	CDrawGraphApp();

	VOID CreateD2DResource(HWND hWnd, ID2D1HwndRenderTarget *&g_pRenderTarget);
	void AddDebugLog(LPCTSTR, COLORREF);
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CDrawGraphView *GetGraphView();
protected:
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	// d2d 
	bool InitEnv();

	ID2D1Factory*			g_pD2DFactory;	// Direct2D factory
	IDWriteFactory*			g_pDWriteFactory; // DWRITE factory;
	CSingleDocTemplate *m_pDocTemplate;


	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDrawGraphApp theApp;