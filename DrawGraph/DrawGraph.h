
// DrawGraph.h : DrawGraph Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CDrawGraphApp:
// �йش����ʵ�֣������ DrawGraph.cpp
//
class CDrawGraphView;
class CDrawGraphApp : public CWinAppEx
{
public:
	CDrawGraphApp();

	VOID CreateD2DResource(HWND hWnd, ID2D1HwndRenderTarget *&g_pRenderTarget);
	void AddDebugLog(LPCTSTR, COLORREF);
// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
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