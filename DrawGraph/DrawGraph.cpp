
// DrawGraph.cpp : 定义应用程序的类行为。
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
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CDrawGraphApp 构造

CDrawGraphApp::CDrawGraphApp()
{
	m_bHiColorIcons = TRUE;
	g_pD2DFactory = NULL;	// Direct2D factory
	g_pDWriteFactory = NULL; // DWRITE factory;
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DrawGraph.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CDrawGraphApp 对象

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

// CDrawGraphApp 初始化

BOOL CDrawGraphApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()	
	AfxInitRichEdit2();
	InitEnv();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("localMFCApp"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDrawGraphDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CDrawGraphView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();

	InitPyEnv();
	return TRUE;
}

int CDrawGraphApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);
	SAFE_RELEASE(g_pD2DFactory);
	UnInitPyEnv();
	return CWinAppEx::ExitInstance();
}

// CDrawGraphApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CDrawGraphApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDrawGraphApp 自定义加载/保存方法

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

// CDrawGraphApp 消息处理程序
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

