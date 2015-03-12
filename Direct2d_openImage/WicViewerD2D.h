// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "Resource.h"

const float DEFAULT_DPI = 96.f;   // Default DPI that maps image resolution directly to screen resoltuion

/******************************************************************
*                                                                 *
*  DemoApp                                                        *
*                                                                 *
******************************************************************/

class DemoApp
{
public:
    DemoApp();
    ~DemoApp();

    HRESULT Initialize(HINSTANCE hInstance);

private:
    HRESULT CreateD2DBitmapFromFile(HWND hWnd);
    bool LocateImageFile(HWND hWnd, LPWSTR pszFileName, DWORD cbFileName);
    HRESULT CreateDeviceResources(HWND hWnd);

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hWnd);
	void OnKeyDown(SHORT vkey);
	void OnMouseWheel(WPARAM wParam);

    static LRESULT CALLBACK s_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	D2D1_POINT_2F GetCursorRealPos();
private:
	HWND					m_hWnd;
    HINSTANCE               m_hInst;
    IWICImagingFactory     *m_pIWICFactory;

    ID2D1Factory           *m_pD2DFactory;
    ID2D1HwndRenderTarget  *m_pRT;
    ID2D1Bitmap            *m_pD2DBitmap;
    IWICFormatConverter    *m_pConvertedSourceBitmap;

	D2D1::Matrix3x2F m_matTrans;
	bool m_bPan;
	FLOAT m_fLastPosX, m_fLastPosY;
};








