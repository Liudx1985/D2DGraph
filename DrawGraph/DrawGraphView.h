
// DrawGraphView.h : CDrawGraphView 类的接口
//

#pragma once


class CDrawGraphView : public CScrollView
{
protected: // 仅从序列化创建
	CDrawGraphView();
	DECLARE_DYNCREATE(CDrawGraphView)

// 特性
public:
	CDrawGraphDoc* GetDocument() const;

// 操作
public:
	void ClearCanvas();
	void SetBrushColor(int iColor);
	void DrawLine(D2D1_POINT_2F const &a, D2D1_VECTOR_2F const &b);
	void DrawRectangle(D2D1_RECT_F const &a);
	void DrawELLIPSE(D2D1_ELLIPSE const &a);

// 重写
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnDraw(CDC* pDC){;}  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
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
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditAddLine();
	afx_msg void OnEditClearCanvas();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // DrawGraphView.cpp 中的调试版本
inline CDrawGraphDoc* CDrawGraphView::GetDocument() const
   { return reinterpret_cast<CDrawGraphDoc*>(m_pDocument); }
#endif

