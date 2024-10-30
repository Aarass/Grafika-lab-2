
// MFCApplication2View.h : interface of the CMFCApplication2View class
//

#pragma once

class CMFCApplication2View : public CView
{
private:
	boolean shouldDrawGrid = false;

private: 
  HENHMETAFILE greenPart;
  HENHMETAFILE yellowPart;

private:
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void DrawBackground(CDC* pDC, int width, int height);
	void DrawFigure(CDC* pDC);
	void DrawGrid(CDC* pDC, int dx, int dy, int width, int height);


protected: // create from serialization only
	CMFCApplication2View() noexcept;
	DECLARE_DYNCREATE(CMFCApplication2View)

// Attributes
public:
	CMFCApplication2Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMFCApplication2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in MFCApplication2View.cpp
inline CMFCApplication2Doc* CMFCApplication2View::GetDocument() const
   { return reinterpret_cast<CMFCApplication2Doc*>(m_pDocument); }
#endif