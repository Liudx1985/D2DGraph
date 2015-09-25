#pragma once



// CScriptFormView 窗体视图

class CScriptFormView : public CFormView
{
	DECLARE_DYNCREATE(CScriptFormView)

	CScriptFormView();           // 动态创建所使用的受保护的构造函数
	virtual ~CScriptFormView();

public:
	void AppendDebugMessage(LPCTSTR str, COLORREF color);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCRIPT_OUTPUT };
#endif
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR, LPCTSTR, DWORD,
		const RECT&, CWnd*, UINT, CCreateContext*);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CEdit m_edtInput;
	CRichEditCtrl m_edtOutput;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
};


