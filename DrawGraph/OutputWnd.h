#pragma once
/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口
#include "ScriptFormView.h"

class COutputList : public CListBox
{
// 构造
public:
	COutputList();

// 实现
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();

	void UpdateFonts();

	void AppendDebugMessage(LPCTSTR lpszMsg, COLORREF color = 0);

// 特性
protected:
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputBuild;
	CScriptFormView *m_pwndOutputDebug;
	COutputList m_wndOutputFind;

protected:

	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

