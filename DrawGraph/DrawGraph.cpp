
// DrawGraph.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "DrawGraph.h"
#include "MainFrm.h"
#include "DrawGraphDoc.h"
#include "ExportAPI.h"
#include "DrawGraphView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDrawGraphApp

BEGIN_MESSAGE_MAP(CDrawGraphApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CDrawGraphApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CDrawGraphApp ����

CDrawGraphApp::CDrawGraphApp()
{
	m_bHiColorIcons = TRUE;
	g_pD2DFactory = NULL;	// Direct2D factory
	g_pDWriteFactory = NULL; // DWRITE factory;
	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DrawGraph.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CDrawGraphApp ����

CDrawGraphApp theApp;

bool CDrawGraphApp::InitEnv()
{
	bool bSupportXM = DirectX::XMVerifyCPUSupport();

	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("Create D2D factory failed!"), _T("Error"), 0);
		return false;
	}
	// Create DirectWrite Factory
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&g_pDWriteFactory)
		);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("Create DirectWrite factory failed!"), _T("Error"), 0);
		return false;
	}

// 	hr = g_pDWriteFactory->CreateTextFormat(
// 		_T("Gabriola"),                   // Font family name
// 		NULL,                          // Font collection(NULL sets it to the system font collection)
// 		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
// 		DWRITE_FONT_STYLE_NORMAL,      // Style
// 		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
// 		25.0f,                         // Size    
// 		_T("en-us"),                      // Local
// 		&g_pTextFormat                 // Pointer to recieve the created object
// 		);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("Create IDWriteTextFormat failed!"), _T("Error"), 0);
		return false;
	}
	return true;
}

// CDrawGraphApp ��ʼ��

BOOL CDrawGraphApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ AfxInitRichEdit2()	
	AfxInitRichEdit2();
	InitEnv();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("localMFCApp"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣  �ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDrawGraphDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CDrawGraphView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ���á�DDE ִ�С�
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// ��������������ָ�������  ���
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	// ������/��
	m_pMainWnd->DragAcceptFiles();

	InitPyEnv();
	return TRUE;
}

int CDrawGraphApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);
	SAFE_RELEASE(g_pD2DFactory);
	UnInitPyEnv();
	return CWinAppEx::ExitInstance();
}

// CDrawGraphApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CDrawGraphApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDrawGraphApp �Զ������/���淽��

void CDrawGraphApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CDrawGraphApp::LoadCustomState()
{
}

void CDrawGraphApp::SaveCustomState()
{
}

void CDrawGraphApp::AddDebugLog(LPCTSTR lpszMsg, COLORREF clr)
{
	CMainFrame *pFrm = dynamic_cast<CMainFrame *>(m_pMainWnd);
	if (pFrm && lpszMsg && _tcslen(lpszMsg))
	{
		pFrm->AddDebugLog(lpszMsg, clr);
	}
}

// CDrawGraphApp ��Ϣ�������
VOID CDrawGraphApp::CreateD2DResource(HWND hWnd, ID2D1HwndRenderTarget *&g_pRenderTarget)
{
	if (!g_pRenderTarget)
	{
		// Create a Direct2D render target
		HRESULT hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				D2D1::SizeU(1024, 900),
				D2D1_PRESENT_OPTIONS_NONE
				),
			&g_pRenderTarget
			);
		if (FAILED(hr))
		{
			MessageBox(hWnd, _T("Create render target failed!"), _T("Error"), 0);
			return;
		}
		g_pRenderTarget->SetTextRenderingParams();
		// Create a brush
// 		hr = g_pRenderTarget->CreateSolidColorBrush(
// 			D2D1::ColorF(D2D1::ColorF::LightGreen),
// 			&g_pGreenBrush
// 			);
// 		if (FAILED(hr))
// 		{
// 			MessageBox(hWnd, _T("Create brush failed!"), _T("Error"), 0);
// 
// 			return;
// 		}
	}
	else
	{
		RECT rcWnd;
		GetClientRect(hWnd, &rcWnd);
		HRESULT hr = g_pRenderTarget->Resize(D2D1::SizeU(
			rcWnd.right - rcWnd.left,
			rcWnd.bottom - rcWnd.top)
			);
		return;
	}
}

