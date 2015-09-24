
// DrawGraphView.cpp : CDrawGraphView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "DrawGraph.h"
#endif

#include "DrawGraphDoc.h"
#include "DrawGraphView.h"
#include "ExportAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDrawGraphView

IMPLEMENT_DYNCREATE(CDrawGraphView, CScrollView)

BEGIN_MESSAGE_MAP(CDrawGraphView, CScrollView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EDIT_ADD, OnEditAddLine)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClearCanvas)
END_MESSAGE_MAP()

// CDrawGraphView ����/����

CDrawGraphView::CDrawGraphView(): m_clrBrush(D2D1::ColorF::LightGreen)
{
	// TODO: �ڴ˴���ӹ������
	m_pRenderTarget = NULL;
	g_pBrush = NULL;
}

CDrawGraphView::~CDrawGraphView()
{
	SAFE_RELEASE(g_pBrush);
	SAFE_RELEASE(m_pRenderTarget);
}

BOOL CDrawGraphView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CScrollView::PreCreateWindow(cs);
}

// CDrawGraphView ����
afx_msg BOOL CDrawGraphView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

afx_msg void CDrawGraphView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	theApp.CreateD2DResource(m_hWnd, m_pRenderTarget);
}

void CDrawGraphView::OnPaint()
{
 	CPaintDC dc(this);

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	if (!m_pRenderTarget)
	{
		return;
	}
	if (!g_pBrush)
	{
		// Create a brush
		HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(
			m_clrBrush,
			&g_pBrush
			);
		if (FAILED(hr))
		{
			MessageBox(_T("Create brush failed!"), _T("Error"), 0);

			return;
		}
	}
	else
	{
		g_pBrush->SetColor(m_clrBrush);
	}
	m_pRenderTarget->BeginDraw();

	// Clear background color to White
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));


	D2D1_SIZE_F szTarget = m_pRenderTarget->GetSize();
	// Draw Rectangle
	m_pRenderTarget->DrawRectangle(
		D2D1::RectF(10, 10, szTarget.width - 10, szTarget.height - 10),
		g_pBrush
		);

	for (auto line: m_vecLines)
	{
		D2D1_POINT_2F &ptStart = line.first;
		D2D1_VECTOR_2F &vecDir = line.second;
		// draw line
		D2D1_POINT_2F ptEnd = ptStart;
		ptEnd.x += vecDir.x;
		ptEnd.y += vecDir.y;
		m_pRenderTarget->DrawLine(ptStart, ptEnd, g_pBrush);
	}

	for (auto rc : m_vecRects)
	{
		m_pRenderTarget->DrawRectangle(rc, g_pBrush);
	}

	for (auto e : m_vecEllipses)
	{
		m_pRenderTarget->DrawEllipse(e, g_pBrush);
	}

	HRESULT hr = m_pRenderTarget->EndDraw();
}

void CDrawGraphView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 2048;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CDrawGraphView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDrawGraphView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CDrawGraphView ���

#ifdef _DEBUG
void CDrawGraphView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDrawGraphView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDrawGraphDoc* CDrawGraphView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDrawGraphDoc)));
	return (CDrawGraphDoc*)m_pDocument;
}
#endif //_DEBUG

// following are test code for script entry.
afx_msg void CDrawGraphView::OnEditAddLine()
{
	// call python script. notice the tab symbols
// 	{
// 		std::string strScript = "DrawLine(10, 10, 500, 500)";
// 		::EvalPyScript(strScript);
// 	}
// 	std::string strScript = "DrawRectangle(50, 50, 100, 100)";
// 	::EvalPyScript(strScript);
// 	{
// 		std::string strScript = "DrawCircle(100, 100, 100)";
// 		::EvalPyScript(strScript);
// 	}
}

afx_msg void CDrawGraphView::OnEditClearCanvas()
{
	// call python script.
	std::string strScript = "ClearCanvas()";
	::EvalPyScript(strScript);
}

void CDrawGraphView::ClearCanvas()
{
	m_vecLines.clear();
	m_vecEllipses.clear();
	m_vecRects.clear();
	InvalidateRect(NULL);
}
void CDrawGraphView::SetBrushColor(int iColor)
{
	m_clrBrush = D2D1::ColorF(iColor);
	InvalidateRect(NULL);
}

void CDrawGraphView::DrawLine(D2D1_POINT_2F const &a, D2D1_VECTOR_2F const &v)
{
	m_vecLines.push_back(std::make_pair(a, v));
	InvalidateRect(NULL);
}

void CDrawGraphView::DrawRectangle(D2D1_RECT_F const &a)
{
	m_vecRects.push_back(a);
	InvalidateRect(NULL);
}

void CDrawGraphView::DrawELLIPSE(D2D1_ELLIPSE const &a)
{
	m_vecEllipses.push_back(a);
	InvalidateRect(NULL);
}
// CDrawGraphView ��Ϣ�������
