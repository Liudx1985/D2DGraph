
// DrawGraphView.h : CDrawGraphView ��Ľӿ�
//

#pragma once


class CDrawGraphView : public CScrollView
{
protected: // �������л�����
	CDrawGraphView();
	DECLARE_DYNCREATE(CDrawGraphView)

// ����
public:
	CDrawGraphDoc* GetDocument() const;

// ����
public:
	void ClearCanvas();
	void SetBrushColor(int iColor);
	void DrawLine(D2D1_POINT_2F const &a, D2D1_VECTOR_2F const &b);
	void DrawRectangle(D2D1_RECT_F const &a);
	void DrawELLIPSE(D2D1_ELLIPSE const &a);

// ��д
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnDraw(CDC* pDC){;}  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CDrawGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	ID2D1HwndRenderTarget*	m_pRenderTarget;	// Render target
	D2D1::ColorF m_clrBrush;
	ID2D1SolidColorBrush*	g_pBrush;	// A black brush, reflect the line color
	std::vector<std::pair<D2D1_POINT_2F, D2D1_VECTOR_2F>> m_vecLines;
	std::vector<D2D1_ELLIPSE> m_vecEllipses;
	std::vector<D2D1_RECT_F> m_vecRects;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditAddLine();
	afx_msg void OnEditClearCanvas();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // DrawGraphView.cpp �еĵ��԰汾
inline CDrawGraphDoc* CDrawGraphView::GetDocument() const
   { return reinterpret_cast<CDrawGraphDoc*>(m_pDocument); }
#endif

