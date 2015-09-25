#pragma once



// CScriptFormView ������ͼ

class CScriptFormView : public CFormView
{
	DECLARE_DYNCREATE(CScriptFormView)

	CScriptFormView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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


