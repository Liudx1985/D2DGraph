#include <windows.h>
#include "tchar.h" 
#include <D2D1.h>
#include <time.h>
#include <cmath>
#include <dwrite.h>
#include <D2d1_1.h>
#include <d2d1helper.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace std;
#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

const UINT32 g_lBackColor = D2D1::ColorF::Black;
const FLOAT pi = 3.1415926;

ID2D1Factory*			g_pD2DFactory = NULL;	// Direct2D factory

ID2D1HwndRenderTarget*	g_pRenderTarget = NULL;	// Render target
ID2D1SolidColorBrush*	g_pBrush = NULL;	// A black brush, reflect the line color

IDWriteFactory*			g_pDWriteFactory = NULL; // DWRITE factory;
IDWriteTextFormat*		g_pTextFormat = NULL;	// text format

VOID CreateD2DResource(HWND hWnd)
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
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::LightGreen),
			&g_pBrush
			);
		if (FAILED(hr))
		{
			MessageBox(hWnd, _T("Create brush failed!"), _T("Error"), 0);

			return;
		}
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


void RenderSierpinski_MetaFunc(D2D1_POINT_2F const &A, D2D1_POINT_2F const &B, D2D1_POINT_2F const &C, int g)
{
	/*
	  C
	  /\
	 B--A
	*/
	if (g > 0)
	{
		g_pRenderTarget->DrawLine(A, B,
			g_pBrush
			);
		g_pRenderTarget->DrawLine(C, B,
			g_pBrush
			);
		g_pRenderTarget->DrawLine(C, A,
			g_pBrush
			);
		D2D1_POINT_2F pt_AC_2 = { fabs(A.x + C.x) / 2,  fabs(A.y + C.y) / 2 };
		D2D1_POINT_2F pt_AB_2 = { fabs(A.x + B.x) / 2,  fabs(A.y + B.y) / 2 };
		D2D1_POINT_2F pt_BC_2 = { fabs(B.x + C.x) / 2,  fabs(B.y + C.y) / 2 };
		
		{RenderSierpinski_MetaFunc(A, pt_AC_2, pt_AB_2, g - 1);}		
		{RenderSierpinski_MetaFunc(B, pt_BC_2, pt_AB_2, g - 1);}
		{RenderSierpinski_MetaFunc(C, pt_AC_2, pt_BC_2, g - 1);}
		
	}
}

void RenderRamus_MetaFunc(D2D1_POINT_2F P, FLOAT alpha, FLOAT L, int n)
{
	if (n > 0)
	{
		ID2D1SolidColorBrush* pBrush = NULL;
		HRESULT hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF((n * n + 8) << 16),
			&pBrush
			);
		D2D1_POINT_2F P_ = { 
			P.x + cos(alpha) * L,
			P.y + sin(alpha) * L 
		};
		//line([x, x1], [y, y1], 'Color', 'g', 'LineWidth', 2);
		g_pRenderTarget->DrawLine(P, P_,
			pBrush, 2.f
			);
		SAFE_RELEASE(pBrush);
		FLOAT alpha_L = alpha + pi / 6;
		FLOAT alpha_R = alpha - pi / 6;
		L = 2 * L / 3;
		RenderRamus_MetaFunc(P_, alpha_L, L, n - 1);
		RenderRamus_MetaFunc(P_, alpha_R, L, n - 1);
	}
}

void RenderSierpinski()
{
	D2D1_SIZE_F szTarget = g_pRenderTarget->GetSize();
	// Draw Rectangle
	D2D1_POINT_2F ptA = {
		szTarget.width - 20, szTarget.height - 20
	};
	D2D1_POINT_2F ptB = {
		20, szTarget.height - 20
	};
	D2D1::Matrix3x2F m;
	D2D1MakeRotateMatrix(60, ptA, &m);
	D2D1_POINT_2F ptC = m.TransformPoint(ptB);
	RenderSierpinski_MetaFunc(ptA, ptB, ptC, 10);
}
/*给定线段AB，科赫曲线可以由以下步骤生成：
(1)将线段分成三等份（AC,CD,DB）
(2)以CD为底，向外（内外随意）画一个等边三角形DMC
(3)将线段CD移去
(4)分别对AC,CM,MD,DB重复1~3。
    M
    /\
A--C--D--B
*/
D2D1_POINT_2F lerp(D2D1_POINT_2F const &A, D2D1_POINT_2F const &B, FLOAT k)
{
	// assert(A != B && k != -1)
	FLOAT k_1 = k + 1;
	return{
		(A.x + B.x * k) / k_1,
		(A.y + B.y * k) / k_1,
	};
}

void RenderSnow_MetaFunc(D2D1_POINT_2F const&A, D2D1_POINT_2F const&B, FLOAT alpha, int n)
{
	if (n <= 0)
	{
		return;
	}
	D2D1_POINT_2F C = lerp(A, B, 0.5);
	D2D1_POINT_2F D = lerp(A, B, 2.f);
	// remove the line C-D
	D2D1::Matrix3x2F m;
	D2D1MakeRotateMatrix(alpha, C, &m);
	D2D1_POINT_2F M = m.TransformPoint(D);
	g_pBrush->SetColor(D2D1::ColorF(g_lBackColor));
	g_pRenderTarget->DrawLine(D, C, g_pBrush, 2.f);
	g_pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::GhostWhite));
	g_pRenderTarget->DrawLine(D, M, g_pBrush);
	g_pRenderTarget->DrawLine(M, C, g_pBrush);

	RenderSnow_MetaFunc(A, C, alpha, n - 1);
	RenderSnow_MetaFunc(C, M, alpha, n - 1);
	RenderSnow_MetaFunc(M, D, alpha, n - 1);
	RenderSnow_MetaFunc(D, B, alpha, n - 1);
}
//
void RenderSnow()
{
	D2D1_SIZE_F szTarget = g_pRenderTarget->GetSize();
	D2D1_POINT_2F A = {
		szTarget.width / 4, szTarget.height * 2 / 3
	};
	D2D1_POINT_2F B = {
		szTarget.width / 4 * 3, szTarget.height * 2 / 3
	};
	D2D1::Matrix3x2F m;
	D2D1MakeRotateMatrix(-60, A, &m);
	D2D1_POINT_2F C = m.TransformPoint(B);

	g_pRenderTarget->DrawLine(A, B, g_pBrush);
	g_pRenderTarget->DrawLine(B, C, g_pBrush);
	g_pRenderTarget->DrawLine(A, C, g_pBrush);

	int n = 5;
	FLOAT fAngle = 60;
	RenderSnow_MetaFunc(A, B, fAngle, n);
	RenderSnow_MetaFunc(B, C, fAngle, n);
	RenderSnow_MetaFunc(C, A, fAngle, n);
}
// Render function.
VOID OnRender(HWND hwnd)
{
	CreateD2DResource(hwnd);
	FLOAT dpiX, dpiY;
	g_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);
	g_pRenderTarget->BeginDraw();

	// Clear background color to White
	g_pRenderTarget->Clear(D2D1::ColorF(g_lBackColor));

	D2D1_SIZE_F szTarget = g_pRenderTarget->GetSize();
	if (1)
	{
		//RenderSierpinski();
		RenderSnow();
	}
	else 
	{
		D2D1_POINT_2F P0 = {
			szTarget.width / 2, szTarget.height - 2
		};
		RenderRamus_MetaFunc(P0, - pi / 2, 150, 15);
	}
	HRESULT hr = g_pRenderTarget->EndDraw();
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("Draw failed!"), _T("Error"), 0);
		return;
	}
}
// Create D2D device in-depended resource. such as 
// ID2D1DrawingStateBlock
// ID2D1Factory
// ID2D1Geometry及继承自它的接口
// ID2D1GeometrySink和ID2D1SimplifiedGeometrySink
// ID2D1StrokeStyle
bool InitEnv()
{
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

	hr = g_pDWriteFactory->CreateTextFormat(
		_T("Gabriola"),                   // Font family name
		NULL,                          // Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,    // Weight
		DWRITE_FONT_STYLE_NORMAL,      // Style
		DWRITE_FONT_STRETCH_NORMAL,    // Stretch
		25.0f,                         // Size    
		_T("en-us"),                      // Local
		&g_pTextFormat                 // Pointer to recieve the created object
		);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("Create IDWriteTextFormat failed!"), _T("Error"), 0);
		return false;
	}
	srand(clock());
	return true;
}


VOID Cleanup()
{
	SAFE_RELEASE(g_pRenderTarget);
	SAFE_RELEASE(g_pBrush);
	SAFE_RELEASE(g_pD2DFactory);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case   WM_PAINT:
		OnRender(hwnd);
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
	}
	break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	WNDCLASSEX winClass;

	winClass.lpszClassName = _T("Direct2D");
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WndProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = NULL;
	winClass.hIconSm = NULL;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;
	winClass.lpszMenuName = NULL;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;

	if (!RegisterClassEx(&winClass))
	{
		MessageBox(NULL, _T("This program requires Windows NT!"), _T("error"), MB_ICONERROR);
		return 0;
	}
	if (!InitEnv())
	{
		return 0;
	}
	HWND hwnd = CreateWindowEx(NULL,
		_T("Direct2D"),					// window class name
		_T("Draw Sierpinski"),			// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		CW_USEDEFAULT,				// initial x position
		CW_USEDEFAULT,				// initial y position
		600,						// initial x size
		600,						// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL);						// creation parameters

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	MSG    msg;
	ZeroMemory(&msg, sizeof(msg));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
