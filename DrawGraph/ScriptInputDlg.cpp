// ScriptInputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DrawGraph.h"
#include "ScriptInputDlg.h"
#include "afxdialogex.h"


// CScriptInputDlg �Ի���

IMPLEMENT_DYNAMIC(CScriptInputDlg, CDialogEx)

CScriptInputDlg::CScriptInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INPUT, pParent)
{

}

CScriptInputDlg::~CScriptInputDlg()
{
}

void CScriptInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCRIPT, m_strScript);
}


BEGIN_MESSAGE_MAP(CScriptInputDlg, CDialogEx)
END_MESSAGE_MAP()


// CScriptInputDlg ��Ϣ�������

void CScriptInputDlg::OnOK()
{
	UpdateData();
	__super::OnOK();
}
