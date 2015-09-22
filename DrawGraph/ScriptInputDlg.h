#pragma once


// CScriptInputDlg 对话框

class CScriptInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScriptInputDlg)

public:
	CScriptInputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScriptInputDlg();
	CString m_strScript;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
