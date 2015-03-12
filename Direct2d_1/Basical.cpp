#include <windows.h>
#include "tchar.h" 
#include <D2D1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory*			g_pD2DFactory	= NULL;	// Direct2D factory

ID2D1HwndRenderTarget*	g_pRenderTarget	= NULL;	// Render target
ID2D1SolidColorBrush*	g_pGreenBrush	= NULL;	// A black brush, reflect the line color

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
			&g_pGreenBrush
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

VOID OnRender(HWND hwnd)
{
	CreateD2DResource(hwnd);
	FLOAT dpiX, dpiY;
	g_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);
	TCHAR sc_helloWorld[64];
	memset(sc_helloWorld, 0, sizeof(sc_helloWorld));
	_stprintf_s(sc_helloWorld, _T("System DPI= %.1f * %.1f"), dpiX, dpiY);
	
	g_pRenderTarget->BeginDraw() ;
	
	// Clear background color to White
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	

	D2D1_SIZE_F szTarget = g_pRenderTarget->GetSize();
	// Draw Rectangle
	g_pRenderTarget->DrawRectangle(
		D2D1::RectF(10, 10, szTarget.width - 10, szTarget.height - 10),
		g_pGreenBrush
		);
	//g_pRenderTarget->FillRectangle();
	g_pRenderTarget->DrawText(
		sc_helloWorld,
		_tcslen(sc_helloWorld),
		g_pTextFormat,
		D2D1::RectF(20, 10, szTarget.width, szTarget.height),
		g_pGreenBrush
		);
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
	return true;
}


VOID Cleanup()
{
	SAFE_RELEASE(g_pRenderTarget) ;
	SAFE_RELEASE(g_pGreenBrush);
	SAFE_RELEASE(g_pD2DFactory);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
    switch (message)    
    {
	case   WM_PAINT:
		OnRender(hwnd) ;
		return 0 ;

	case WM_KEYDOWN: 
		{ 
			switch( wParam ) 
			{ 
			case VK_ESCAPE: 
				SendMessage( hwnd, WM_CLOSE, 0, 0 ); 
				break ; 
			default: 
				break ; 
			} 
		} 
		break ; 

	case WM_DESTROY: 
		Cleanup(); 
		PostQuitMessage( 0 ); 
		return 0; 
    }

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
        
    WNDCLASSEX winClass ;

	winClass.lpszClassName = _T("Direct2D");
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WndProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;
	
	if (!RegisterClassEx (&winClass))   
	{
		MessageBox ( NULL, _T( "This program requires Windows NT!" ), _T("error"), MB_ICONERROR) ;
		return 0 ;  
    }   
	if (!InitEnv())
	{ 
		return 0;
	}
	HWND hwnd = CreateWindowEx(NULL,  
		_T("Direct2D"),					// window class name
		_T("Draw Rectangle"),			// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		CW_USEDEFAULT,				// initial x position
		CW_USEDEFAULT,				// initial y position
		600,						// initial x size
		600,						// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

        ShowWindow (hwnd, iCmdShow) ;
		UpdateWindow (hwnd) ;

		MSG    msg ;  
		ZeroMemory(&msg, sizeof(msg)) ;

		while (GetMessage (&msg, NULL, 0, 0))  
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}

		return msg.wParam ;
}
