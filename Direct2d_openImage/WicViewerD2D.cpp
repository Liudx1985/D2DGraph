// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <cmath>
#include <d2d1.h>
#include "WICViewerD2D.h"

template <typename T>
inline void SafeRelease(T *&p)
{
    if (nullptr != p)
    {
        p->Release();
        p = nullptr;
    }
}
#ifndef D3DX_PI
const float D3DX_PI = 3.1415926f;
#endif
/******************************************************************
*  Application entrypoint                                         *
******************************************************************/

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR pszCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pszCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr))
    {
        {
            DemoApp app;
            hr = app.Initialize(hInstance);

            if (SUCCEEDED(hr))
            {
                BOOL fRet;
                MSG msg;

                // Main message loop:
                while ((fRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
                {
                    if (fRet == -1)
                    {
                        break;
                    }
                    else
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
            }
        }
        CoUninitialize();
    }

    return 0;
}

/******************************************************************
*  Initialize member data                                         *
******************************************************************/

DemoApp::DemoApp()
    :
    m_pD2DBitmap(nullptr),
    m_pConvertedSourceBitmap(nullptr),
    m_pIWICFactory(nullptr),
    m_pD2DFactory(nullptr),
    m_pRT(nullptr),
	m_fZoomRate(1),
	m_hWnd(0),
	m_fRotate(0),
	m_fOffX(0),
	m_fOffY(0)
{
}

/******************************************************************
*  Tear down resources                                            *
******************************************************************/

DemoApp::~DemoApp()
{
    SafeRelease(m_pD2DBitmap);
    SafeRelease(m_pConvertedSourceBitmap);
    SafeRelease(m_pIWICFactory);
    SafeRelease(m_pD2DFactory);
    SafeRelease(m_pRT);
}

/******************************************************************
*  Create application window and resources                        *
******************************************************************/

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
    
    HRESULT hr = S_OK;

    // Create WIC factory
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pIWICFactory)
        );

    if (SUCCEEDED(hr))
    {
        // Create D2D factory
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    }

    if (SUCCEEDED(hr))
    {
        WNDCLASSEX wcex;

        // Register window class
        wcex.cbSize        = sizeof(WNDCLASSEX);
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = DemoApp::s_WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = hInstance;
        wcex.hIcon         = nullptr;
        wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
        wcex.lpszClassName = L"WICViewerD2D";
        wcex.hIconSm       = nullptr;

        m_hInst = hInstance;

        hr = RegisterClassEx(&wcex) ? S_OK : E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        // Create window
		m_hWnd = CreateWindow(
            L"WICViewerD2D",
            L"WIC Viewer D2D Sample",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            640,
            480,
            nullptr,
            nullptr,
            hInstance,
            this
            );

        hr = m_hWnd ? S_OK : E_FAIL;
    }

    return hr;
}

/******************************************************************
*  Load an image file and create an D2DBitmap                     *
******************************************************************/

HRESULT DemoApp::CreateD2DBitmapFromFile(HWND hWnd)
{
    HRESULT hr = S_OK;

    WCHAR szFileName[MAX_PATH];

    // Step 1: Create the open dialog box and locate the image file
    if (LocateImageFile(hWnd, szFileName, ARRAYSIZE(szFileName)))
    {
        // Step 2: Decode the source image

        // Create a decoder
        IWICBitmapDecoder *pDecoder = nullptr;

        hr = m_pIWICFactory->CreateDecoderFromFilename(
            szFileName,                      // Image to be decoded
            nullptr,                         // Do not prefer a particular vendor
            GENERIC_READ,                    // Desired read access to the file
            WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
            &pDecoder                        // Pointer to the decoder
            );

        // Retrieve the first frame of the image from the decoder
        IWICBitmapFrameDecode *pFrame = nullptr;

        if (SUCCEEDED(hr))
        {
            hr = pDecoder->GetFrame(0, &pFrame);
        }

        //Step 3: Format convert the frame to 32bppPBGRA
        if (SUCCEEDED(hr))
        {
            SafeRelease(m_pConvertedSourceBitmap);
            hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pConvertedSourceBitmap->Initialize(
                pFrame,                          // Input bitmap to convert
                GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
                WICBitmapDitherTypeNone,         // Specified dither pattern
                nullptr,                         // Specify a particular palette 
                0.f,                             // Alpha threshold
                WICBitmapPaletteTypeCustom       // Palette translation type
                );
        }

        //Step 4: Create render target and D2D bitmap from IWICBitmapSource
        if (SUCCEEDED(hr))
        {
            hr = CreateDeviceResources(hWnd);
        }

        if (SUCCEEDED(hr))
        {
            // Need to release the previous D2DBitmap if there is one
            SafeRelease(m_pD2DBitmap);
            hr = m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, nullptr, &m_pD2DBitmap);
        }

        SafeRelease(pDecoder);
        SafeRelease(pFrame);
    }

    return hr;
}

/******************************************************************
* Creates an open file dialog box and locate the image to decode. *
******************************************************************/

bool DemoApp::LocateImageFile(HWND hWnd, LPWSTR pszFileName, DWORD cchFileName)
{
    pszFileName[0] = L'\0';

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = hWnd;
    ofn.lpstrFilter     = L"All Image Files\0"              L"*.bmp;*.dib;*.wdp;*.mdp;*.hdp;*.gif;*.png;*.jpg;*.jpeg;*.tif;*.ico\0"
                          L"Windows Bitmap\0"               L"*.bmp;*.dib\0"
                          L"High Definition Photo\0"        L"*.wdp;*.mdp;*.hdp\0"
                          L"Graphics Interchange Format\0"  L"*.gif\0"
                          L"Portable Network Graphics\0"    L"*.png\0"
                          L"JPEG File Interchange Format\0" L"*.jpg;*.jpeg\0"
                          L"Tiff File\0"                    L"*.tif\0"
                          L"Icon\0"                         L"*.ico\0"
                          L"All Files\0"                    L"*.*\0"
                          L"\0";
    ofn.lpstrFile       = pszFileName;
    ofn.nMaxFile        = cchFileName;
    ofn.lpstrTitle      = L"Open Image";
    ofn.Flags           = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    // Display the Open dialog box. 
    return (GetOpenFileName(&ofn) == TRUE);
}


/******************************************************************
*  This method creates resources which are bound to a particular  *
*  D2D device. It's all centralized here, in case the resources   *
*  need to be recreated in the event of D2D device loss           *
* (e.g. display change, remoting, removal of video card, etc).    *
******************************************************************/

HRESULT DemoApp::CreateDeviceResources(HWND hWnd)
{
    HRESULT hr = S_OK;

    if (!m_pRT)
    {
        RECT rc;
        hr = GetClientRect(hWnd, &rc) ? S_OK : E_FAIL;

        if (SUCCEEDED(hr))
        {
            auto renderTargetProperties = D2D1::RenderTargetProperties();

            // Set the DPI to be the default system DPI to allow direct mapping
            // between image pixels and desktop pixels in different system DPI settings
            renderTargetProperties.dpiX = DEFAULT_DPI;
            renderTargetProperties.dpiY = DEFAULT_DPI;

            auto size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

            hr = m_pD2DFactory->CreateHwndRenderTarget(
                renderTargetProperties,
                D2D1::HwndRenderTargetProperties(hWnd, size),
                &m_pRT
                );
        }
    }

    return hr;
}

/******************************************************************
*  Regsitered Window message handler                              *
******************************************************************/

LRESULT CALLBACK DemoApp::s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DemoApp *pThis;
    LRESULT lRet = 0;

    if (uMsg == WM_NCCREATE)
    {
        auto pcs = reinterpret_cast<LPCREATESTRUCT> (lParam);
        pThis = reinterpret_cast<DemoApp *> (pcs->lpCreateParams);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (pThis));
        lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    else
    {
        pThis = reinterpret_cast<DemoApp *> (GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (pThis)
        {
            lRet = pThis->WndProc(hWnd, uMsg, wParam, lParam);
        }
        else
        {
            lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
    return lRet;
}

/******************************************************************
*  Internal Window message handler                                *
******************************************************************/

LRESULT DemoApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
        case WM_COMMAND:
        {
            // Parse the menu selections:
            switch (LOWORD(wParam))
            {
                case IDM_FILE:
                {
                    if (SUCCEEDED(CreateD2DBitmapFromFile(hWnd)))
                    {
                        InvalidateRect(hWnd, nullptr, TRUE);
                    }
                    else
                    {
                        MessageBox(hWnd, L"Failed to load image, select a new one.", L"Application Error", MB_ICONEXCLAMATION | MB_OK);
                    }
                    break;
                }
                case IDM_EXIT:
                {
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;
                }
            }
            break;
        }
        case WM_SIZE:
        {
            auto size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));

            if (m_pRT)
            {
                // If we couldn't resize, release the device and we'll recreate it
                // during the next render pass.
                if (FAILED(m_pRT->Resize(size)))
                {
                    SafeRelease(m_pRT);
                    SafeRelease(m_pD2DBitmap);
                }
            }
            break;
        }
        case WM_PAINT:
        {
            return OnPaint(hWnd);
        }
		case WM_KEYDOWN:
		{
			OnKeyDown(static_cast<SHORT>(wParam));
			break;
		}
		case WM_MOUSEWHEEL:
		{
			OnMouseWheel(wParam);
			break;
		}
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

/******************************************************************
* Rendering routine using D2D                                     *
******************************************************************/
LRESULT DemoApp::OnPaint(HWND hWnd)
{
    HRESULT hr = S_OK;
    PAINTSTRUCT ps;

    if (BeginPaint(hWnd, &ps))
    {
        // Create render target if not yet created
        hr = CreateDeviceResources(hWnd);

        if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
        {
            m_pRT->BeginDraw();
			auto rtSize = m_pRT->GetSize();
			RECT rcSize;
			GetClientRect(hWnd, &rcSize);
			rtSize.width = rcSize.right - rcSize.left;
			rtSize.height = rcSize.bottom - rcSize.top;
			m_pRT->SetTransform(
				D2D1::Matrix3x2F::Translation(m_fOffX, m_fOffY) *
				D2D1::Matrix3x2F::Scale((float)m_fZoomRate, (float)m_fZoomRate,
					D2D1::Point2F(rtSize.width / 2, rtSize.height / 2)
					) *
				D2D1::Matrix3x2F::Rotation((float)m_fRotate,
					D2D1::Point2F(rtSize.width / 2, rtSize.height / 2)
					)
				);
			
            // Clear the background
            m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
			
            // D2DBitmap may have been released due to device loss. 
            // If so, re-create it from the source bitmap
            if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
            {
                m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, nullptr, &m_pD2DBitmap);				
            }
			if (m_pD2DBitmap)
			{
				rtSize = m_pD2DBitmap->GetSize();
			}
			// Create a rectangle with size of current window
			auto rectangle = D2D1::RectF(10.0f, 10.0f, rtSize.width - 10, rtSize.height - 10);

            // Draws an image and scales it to the current window size
            if (m_pD2DBitmap)
            {
                m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
            }

            hr = m_pRT->EndDraw();

            // In case of device loss, discard D2D render target and D2DBitmap
            // They will be re-created in the next rendering pass
            if (hr == D2DERR_RECREATE_TARGET)
            {
                SafeRelease(m_pD2DBitmap);
                SafeRelease(m_pRT);
                // Force a re-render
                hr = InvalidateRect(hWnd, nullptr, TRUE)? S_OK : E_FAIL;
            }
        }

        EndPaint(hWnd, &ps);
    }

    return SUCCEEDED(hr) ? 0 : 1;
}  


void DemoApp::OnKeyDown(SHORT vkey)
{
	const float fDeleta = 50;
	switch (vkey)
	{
	case VK_LEFT:
		m_fOffX -= fDeleta;
		break;
	case VK_RIGHT:
		m_fOffX += fDeleta;
		break;
	case VK_UP:
		m_fOffY -= fDeleta;
		break;
	case VK_DOWN:
		m_fOffY += fDeleta;
		break;
	case VK_BACK:
		m_fOffX = m_fOffY = m_fRotate = 0;
		m_fZoomRate = 1;
		break;
	}
	InvalidateRect(m_hWnd, NULL, FALSE);
}

void DemoApp::OnMouseWheel(WPARAM wParam)
{
	WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
	FLOAT zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	if (fwKeys & MK_CONTROL)
	{
		m_fRotate += 30 * zDelta / WHEEL_DELTA;
	}
	else
	{
		m_fZoomRate *= powf(2.0f, zDelta / WHEEL_DELTA);
	}
	InvalidateRect(m_hWnd, NULL, FALSE);
}