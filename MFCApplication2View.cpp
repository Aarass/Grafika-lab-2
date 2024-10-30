
// MFCApplication2View.cpp : implementation of the CMFCApplication2View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCApplication2.h"
#endif

#include "MFCApplication2Doc.h"
#include "MFCApplication2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication2View

IMPLEMENT_DYNCREATE(CMFCApplication2View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CMFCApplication2View construction/destruction

CMFCApplication2View::CMFCApplication2View() noexcept
{
	CString green = CString("./res/cactus_part.emf");
	CString yellow = CString("./res/cactus_part_light.emf");

	this->greenPart = GetEnhMetaFile(green.GetBuffer(green.GetLength()));
	this->yellowPart = GetEnhMetaFile(yellow.GetBuffer(yellow.GetLength()));
}

CMFCApplication2View::~CMFCApplication2View()
{
}

BOOL CMFCApplication2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMFCApplication2View printing

BOOL CMFCApplication2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCApplication2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCApplication2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CMFCApplication2View diagnostics

#ifdef _DEBUG
void CMFCApplication2View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication2Doc* CMFCApplication2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication2Doc)));
	return (CMFCApplication2Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication2View message handlers

void CMFCApplication2View::OnDraw(CDC* pDC)
{
	CMFCApplication2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

  const auto width = 500;
	const auto height = 500;
	const auto cellSizeX = width / 20;
	const auto cellSizeY = height / 20;


	DrawBackground(pDC, width, height);
	DrawFigure(pDC);

	if (shouldDrawGrid) {
		DrawGrid(pDC, cellSizeX, cellSizeY, width, height);
	}
}

void CMFCApplication2View::DrawBackground(CDC* pDC, int width, int height)
{
	CBrush brush(RGB(135,206,235));

	auto oldPen = pDC->SelectStockObject(NULL_PEN);
	auto oldBrush = pDC->SelectObject(brush);

	pDC->Rectangle(0, 0, width, height);

	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

void CMFCApplication2View::DrawFigure(CDC* pDC)
{
	pDC->PlayMetaFile(greenPart, CRect(0,0, 100, 100)); 
}

void CMFCApplication2View::DrawGrid(CDC* pDC, int dx, int dy, int width, int height)
{
	auto oldPen = pDC->SelectStockObject(WHITE_PEN);

	for (int x = 0; x <= width; x += dx) {
		pDC->MoveTo(CPoint(x, 0));
		pDC->LineTo(CPoint(x, height));
	}

	for (int y = 0; y <= width; y += dy) {
		pDC->MoveTo(CPoint(0, y));
		pDC->LineTo(CPoint(width, y));
	}

	pDC->SelectObject(oldPen);
}

void CMFCApplication2View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM xForm;
	xForm.eDx = dX;
	xForm.eDy = dY;
	pDC->ModifyWorldTransform(&xForm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CMFCApplication2View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM xForm;
	xForm.eM11 = sX;
	xForm.eM22 = sY;
	pDC->ModifyWorldTransform(&xForm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CMFCApplication2View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	XFORM xForm;
	xForm.eM11 = cos(angle);
	xForm.eM12 = sin(angle);
	xForm.eM21 = -sin(angle);
	xForm.eM22 = cos(angle);
	pDC->ModifyWorldTransform(&xForm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CMFCApplication2View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN) {
		shouldDrawGrid = !shouldDrawGrid;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
