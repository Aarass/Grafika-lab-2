
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
#include <vector>

#define PI 3.14159265358979323846

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// class Edge;
// class Node;

// class Node {
// private:
// 	std::vector<Edge*> children;

// public:
// 	Node(int capacity) { 
// 		children.reserve(capacity);
// 	}

// 	void AddChild(Edge* edge);
// 	void Draw(CDC* pDC);
// };

// class Edge {
// private:
// 	Node* parrent;
// 	Node* next;
// 	boolean controllable;

// public:
// 	Edge(Node* parrent, boolean controllable) 
// 		:parrent(parrent),
// 		 controllable(controllable),
// 		 next(nullptr)
// 	{
// 		parrent->AddChild(this);
// 	}

// 	 void AttachNode(Node* next); 
// 	 void Draw(CDC* pDC);
// };

// void Node::Draw(CDC* pDC) {
// 		// Draw self
// 		for (auto child : children) {
// 			child->Draw(pDC);
// 		}
// }

// void Node::AddChild(Edge* edge) {
// 	children.emplace_back(edge);
// }


// void Edge::AttachNode(Node* next)
// {
// 	this->next = next;
// }

// void Edge::Draw(CDC* pDC) {
// 	//Draw self
// 	if (next != nullptr) {
// 		next->Draw(pDC);
// 	}
// }

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

	DrawBackground(pDC);
	DrawFigure(pDC);

	if (shouldDrawGrid) {
		DrawGrid(pDC, cellSizeX, cellSizeY, width, height);
	}
}

void CMFCApplication2View::DrawBackground(CDC* pDC)
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
	int oldGraphicsMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldTransform; pDC->GetWorldTransform(&oldTransform);


	Translate(pDC, width / 2, height, true);


	// // Translate(pDC, width / 2, height);
	// // Translate(pDC, 100, 0);
	// Scale(pDC, .3f, 1.0f);
	// Rotate(pDC, PI / 4);
	// pDC->Rectangle(-25, -25, 25, 25);

	// return;

	// Node n1(1), n2(3), n3(2), n4(2), n5(1), n6(2);
	// Edge e1(&n1, true), e2(&n2, false), e3(&n2, false), e4(&n2, false);
	// e1.AttachNode(&n2);
	// n1.Draw(pDC);

	CBrush brush(RGB(0, 204, 0));
	pDC->SelectObject(brush);

	XFORM tmpTransform; 
	std::vector<XFORM> junctions;
	junctions.reserve(6);


	const int r = cellSizeX / 2 - 2;
	const int defaultHalfWidth = 1.3f * cellSizeX;
	const int defaultHeight = 3 * cellSizeY;

	Translate(pDC, 0, -3 * cellSizeY);

	pDC->GetWorldTransform(&tmpTransform);
	junctions.push_back(tmpTransform);
	// pDC->Ellipse(-r, -r, r, r);
	Rotate(pDC, angle1);
	{
		pDC->PlayMetaFile(yellowPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
		Translate(pDC, 0, -defaultHeight);
		pDC->GetWorldTransform(&tmpTransform);
		junctions.push_back(tmpTransform);
		// pDC->Ellipse(-r, -r, r, r);

		Scale(pDC, 1.0f / 3.0f, 1.0f);
		pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
		Scale(pDC, 3.0f, 1.0f);



		// Leva strana
		Rotate(pDC, -PI / 4);
		{
			Scale(pDC, 1.0f / 3.0f, 1.0f);
			pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
			Scale(pDC, 3.0f, 1.0f);

			Translate(pDC, 0, -3 * cellSizeY);
			{
				pDC->GetWorldTransform(&tmpTransform);
				junctions.push_back(tmpTransform);
				// pDC->Ellipse(-r, -r, r, r);

				Rotate(pDC, -PI / 4);
				{
					Scale(pDC, 0.7f, 1.0f);
					pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
					Scale(pDC, 1.0f / 0.7f, 1.0f);
				}
				Rotate(pDC, PI / 4);


				Rotate(pDC, PI / 4);
				{
					Scale(pDC, 0.7f, 1.0f);
					pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
					Scale(pDC, 1.0f / 0.7f, 1.0f);

					Translate(pDC, 0, -3 * cellSizeY);
					{
						pDC->GetWorldTransform(&tmpTransform);
						junctions.push_back(tmpTransform);
						// pDC->Ellipse(-r, -r, r, r);

						// Scale(pDC, 0.5f, 1.0f);
						pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
						// Scale(pDC, 2.0f, 1.0f);
					}
					Translate(pDC, 0, 3 * cellSizeY);
				}
				Rotate(pDC, -PI / 4);

			}
			Translate(pDC, 0, 3 * cellSizeY);

		}
		Rotate(pDC, PI / 4);

		// Desna strana
		Rotate(pDC, PI / 4);
		{
			Scale(pDC, 1.0f / 3.0f, 1.0f);
			pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
			Scale(pDC, 3.0f, 1.0f);

			Translate(pDC, 0, -3 * cellSizeY);
			pDC->GetWorldTransform(&tmpTransform);
			junctions.push_back(tmpTransform);
			// pDC->Ellipse(-r, -r, r, r);

			Rotate(pDC, -PI / 4 + angle2);
			{
				Scale(pDC, 0.7f, 1.0f);
				pDC->PlayMetaFile(yellowPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
				Scale(pDC, 1.0f / 0.7f, 1.0f);
			}
			Rotate(pDC, PI / 4 - angle2);


			Rotate(pDC, PI / 4);
			{
				Scale(pDC, 0.7f, 1.0f);
				pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
				Scale(pDC, 1.0f / 0.7f, 1.0f);

				Translate(pDC, 0, -3 * cellSizeY);
				pDC->GetWorldTransform(&tmpTransform);
				junctions.push_back(tmpTransform);
				// pDC->Ellipse(-r, -r, r, r);

				Rotate(pDC, -PI / 4);
				{
					Scale(pDC, 0.7f, 1.0f);
					pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
					Scale(pDC, 1.0f / 0.7f, 1.0f);
				}
				Rotate(pDC, PI / 4);


				Rotate(pDC, PI / 4);
				{
					Scale(pDC, 0.7f, 1.0f);
					pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
					Scale(pDC, 1.0f / 0.7f, 1.0f);
				}
				Rotate(pDC, -PI / 4);
			}
			Rotate(pDC, -PI / 4);
		}
		Rotate(pDC, -PI / 4);
	}

	for (auto& junction : junctions) {
		pDC->SetWorldTransform(&junction);
		pDC->Ellipse(-r, -r, r, r);
	}

	pDC->SetWorldTransform(&oldTransform);
	pDC->SetGraphicsMode(oldGraphicsMode);

	CBrush potBrush(RGB(222,148,0));

	pDC->SelectStockObject(BLACK_PEN);
	pDC->SelectObject(potBrush);

	pDC->Rectangle(7.5f * cellSizeX, 17 * cellSizeY, 12.5f * cellSizeX ,18 * cellSizeY);

	POINT lpPoints[] = {
		POINT {8 * cellSizeX, 18 * cellSizeY},
		POINT {12 * cellSizeX, 18 * cellSizeY},
		POINT {(long)(11.5 * cellSizeX), 20 * cellSizeY},
		POINT {(long)(8.5 * cellSizeX), 20 * cellSizeY},
	};

	pDC->Polygon(lpPoints, 4);
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
	xForm.eM11 = 1.0f;
	xForm.eM12 = 0.0f;
	xForm.eM21 = 0.0f;
	xForm.eM22 = 1.0f;
	xForm.eDx = dX;
	xForm.eDy = dY;
	pDC->ModifyWorldTransform(&xForm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CMFCApplication2View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM xForm;
	xForm.eM11 = sX;
	xForm.eM12 = 0.0f;
	xForm.eM21 = 0.0f;
	xForm.eM22 = sY;
	xForm.eDx = 0.0f;
	xForm.eDy = 0.0f;
	pDC->ModifyWorldTransform(&xForm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CMFCApplication2View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	XFORM xForm;
	xForm.eM11 = cos(angle);
	xForm.eM12 = sin(angle);
	xForm.eM21 = -sin(angle);
	xForm.eM22 = cos(angle);
	xForm.eDx = 0.0f;
	xForm.eDy = 0.0f;
	pDC->ModifyWorldTransform(&xForm, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CMFCApplication2View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
		case VK_RETURN:
			shouldDrawGrid = !shouldDrawGrid;
			break;
		case 'D':
			angle1 += PI / 32;
			break;
		case 'A':
			angle1 -= PI / 32;
			break;
		case 'E':
			angle2 += PI / 32;
			break;
		case 'Q':
			angle2 -= PI / 32;
			break;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
