// ScriptFormView.cpp : 实现文件
//

#include "stdafx.h"
#include "DrawGraph.h"
#include "ExportAPI.h"
#include "ScriptFormView.h"


// CScriptFormView

IMPLEMENT_DYNCREATE(CScriptFormView, CFormView)

CScriptFormView::CScriptFormView()
	: CFormView(IDD_SCRIPT_OUTPUT)
{

}

CScriptFormView::~CScriptFormView()
{
}

void CScriptFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_edtInput);
	DDX_Control(pDX, IDC_REDIT_SCRIPT, m_edtOutput);
}

BEGIN_MESSAGE_MAP(CScriptFormView, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CScriptFormView 诊断

#ifdef _DEBUG
void CScriptFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CScriptFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CScriptFormView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
	//m_edtOutput.SetReadOnly();
	m_edtOutput.SetTextMode(0);
	SetScrollSizes(MM_TEXT, { 1, 1 });
	OnSize(0, 0, 0);
}

BOOL CScriptFormView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
	DWORD dwRequestedStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext)
{
	return 
		__super::Create(lpszClassName, lpszWindowName, dwRequestedStyle, rect, pParentWnd, nID, pContext);
}

int GetNumVisibleLines(CRichEditCtrl* pCtrl)
{
	CRect rect;
	long nFirstChar, nLastChar;
	long nFirstLine, nLastLine;

	// Get client rect of rich edit control
	pCtrl->GetClientRect(rect);

	// Get character index close to upper left corner
	nFirstChar = pCtrl->CharFromPos(CPoint(0, 0));

	// Get character index close to lower right corner
	nLastChar = pCtrl->CharFromPos(CPoint(rect.right, rect.bottom));
	if (nLastChar < 0)
	{
		nLastChar = pCtrl->GetTextLength();
	}

	// Convert to lines
	nFirstLine = pCtrl->LineFromChar(nFirstChar);
	nLastLine = pCtrl->LineFromChar(nLastChar);

	return (nLastLine - nFirstLine);
}

void CScriptFormView::AppendDebugMessage(LPCTSTR str, COLORREF color)
{
	long nVisible = 0;
	long nInsertionPoint = 0;
	CHARFORMAT cf;

	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR

	cf.crTextColor = color;

	// Set insertion point to end of text
	nInsertionPoint = m_edtOutput.GetWindowTextLength();
	m_edtOutput.SetSel(nInsertionPoint, -1);

	// Set the character format
	m_edtOutput.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing 
	// selected, this will simply insert
	// the string at the current caret position.
	m_edtOutput.ReplaceSel(str);

	// Get number of currently visible lines or maximum number of visible lines
	// (We must call GetNumVisibleLines() before the first call to LineScroll()!)
	nVisible = GetNumVisibleLines(&m_edtOutput);

	// Now this is the fix of CRichEditCtrl's abnormal behaviour when used
	// in an application not based on dialogs. Checking the focus prevents
	// us from scrolling when the CRichEditCtrl does so automatically,
	// even though ES_AUTOxSCROLL style is NOT set.
	if (&m_edtOutput != m_edtOutput.GetFocus())
	{
		m_edtOutput.LineScroll(INT_MAX);
		m_edtOutput.LineScroll(1 - nVisible);
	}
}
// CScriptFormView 消息处理程序

afx_msg void CScriptFormView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	CRect rcWnd;
	GetClientRect(&rcWnd);
	if (m_edtInput.GetSafeHwnd())
	{
		CRect rcEdt;
		m_edtInput.GetClientRect(&rcEdt);
		int iHei = rcEdt.Height();
		rcEdt.left = rcWnd.left;
		rcEdt.right = rcWnd.right;
		rcEdt.bottom = rcWnd.bottom;
		rcEdt.top = rcEdt.bottom - iHei;
		m_edtInput.MoveWindow(&rcEdt);
		rcWnd.bottom = rcEdt.top - 4;
	}
	if (m_edtOutput.GetSafeHwnd())
	{
		m_edtOutput.MoveWindow(&rcWnd);
	}
}

BOOL CScriptFormView::PreTranslateMessage(MSG* pMsg) {
	if ((pMsg->message == WM_CHAR) && (pMsg->wParam == VK_RETURN)) {
		
		if (pMsg->hwnd == m_edtInput.m_hWnd) {
			CString strScript;
			m_edtInput.GetWindowText(strScript);
			if (::EvalPyScript((LPCTSTR)strScript))
			{
				m_edtInput.SetWindowTextA("");
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}