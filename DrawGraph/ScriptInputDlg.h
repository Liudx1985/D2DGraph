#pragma once


// CScriptInputDlg �Ի���

class CScriptInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScriptInputDlg)

public:
	CScriptInputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScriptInputDlg();
	CString m_strScript;
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
