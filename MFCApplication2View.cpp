
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

#include <functional>

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

struct Part {
	double rotation;
	bool controllable = false;
	std::vector<Part*> children;
};




// CMFCApplication2View

IMPLEMENT_DYNCREATE(CMFCApplication2View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
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

	const int oldGraphicsMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM originalTransform; pDC->GetWorldTransform(&originalTransform);

	Scale(pDC, scale, scale, true);
	if (shouldFollowMouse) {
		Translate(pDC, offset.x, offset.y,true);
	} else {
		Translate(pDC, width / 2, height - 3 * cellSizeY, true);
	}

	DrawFigure(pDC);

	pDC->SetWorldTransform(&originalTransform);
	pDC->SetGraphicsMode(oldGraphicsMode);

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
	const int partWidth = 2.6f * cellSizeX;
	const int partHeight = 3 * cellSizeY;
	const int r = cellSizeX / 2 - 2;

	std::function<void(Part, float)> draw = [&](const Part& p, float scale) {
		XFORM oldTransform; pDC->GetWorldTransform(&oldTransform);

		Rotate(pDC, p.rotation);

		if (p.controllable) {
			pDC->PlayMetaFile(yellowPart, CRect(-(partWidth >> 1) * scale, 0, (partWidth >> 1) * scale, -partHeight)); 
		} else {
			pDC->PlayMetaFile(greenPart, CRect(-(partWidth >> 1) * scale, 0, (partWidth >> 1) * scale, -partHeight)); 
		}

		Translate(pDC, 0, -partHeight);

		if (p.children.size()) {
			float scaleFactor = 1.0f;
			switch (p.children.size()) {
				case 2:
					scaleFactor = 0.7f;
					break;
				case 3:
					scaleFactor = 1.0f / 3.0f;
					break;
			}

			for (auto c : p.children) {
				draw(*c, scaleFactor);
			}

			pDC->Ellipse(-r, -r, r, r);
		}

		pDC->SetWorldTransform(&oldTransform);
	};


	Part p1{angle1, true}, p2{-PI / 4}, p3{0.0}, p4{PI/4}, p5{-PI/4}, p6{PI/4}, p7{0.0f}, p8{-PI / 4 + angle2, true}, p9{PI / 4}, p10{-PI / 4}, p11{PI / 4};

	p1.children.reserve(3);
	p1.children.push_back(&p2);
	p1.children.push_back(&p3);
	p1.children.push_back(&p4);
	p2.children.push_back(&p5);
	p2.children.push_back(&p6);
	p6.children.push_back(&p7);
	p4.children.push_back(&p8);
	p4.children.push_back(&p9);
	p9.children.push_back(&p10);
	p9.children.push_back(&p11);


	CBrush greenBrush(RGB(0, 204, 0)), potBrush(RGB(222,148,0));

	pDC->SelectStockObject(BLACK_PEN);
	pDC->SelectObject(greenBrush);

	draw(p1, 1.0f);
	pDC->Ellipse(-r, -r, r, r);

	pDC->SelectObject(potBrush);

	POINT lpPoints[] = {
		POINT {-2 * cellSizeX, 1 * cellSizeY},
		POINT {2 * cellSizeX, 1 * cellSizeY},
		POINT {(long)(1.5 * cellSizeX), 3 * cellSizeY},
		POINT {(long)(-1.5 * cellSizeX), 3 * cellSizeY},
	};

	pDC->Rectangle(-2.5f * cellSizeX, 0.0f, 2.5f * cellSizeX, 1 * cellSizeY);
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
		case VK_SPACE:
			shouldFollowMouse = !shouldFollowMouse;
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

void CMFCApplication2View::OnMouseMove(UINT nFlags, CPoint point)
{
	offset = point;

	if (shouldFollowMouse) {
		Invalidate();
	}

	CView::OnMouseMove(nFlags, point);
}


BOOL CMFCApplication2View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	scale += zDelta * 0.001f;
	Invalidate();

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

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












	// Rotate(pDC, angle1);
	// {
	// 	pDC->PlayMetaFile(yellowPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 	Translate(pDC, 0, -defaultHeight);
	// 	// pDC->Ellipse(-r, -r, r, r);

	// 	Scale(pDC, 1.0f / 3.0f, 1.0f);
	// 	pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 	Scale(pDC, 3.0f, 1.0f);



	// 	// Leva strana
	// 	Rotate(pDC, -PI / 4);
	// 	{
	// 		Scale(pDC, 1.0f / 3.0f, 1.0f);
	// 		pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 		Scale(pDC, 3.0f, 1.0f);

	// 		Translate(pDC, 0, -3 * cellSizeY);
	// 		{
	// 			// pDC->Ellipse(-r, -r, r, r);

	// 			Rotate(pDC, -PI / 4);
	// 			{
	// 				Scale(pDC, 0.7f, 1.0f);
	// 				pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 				Scale(pDC, 1.0f / 0.7f, 1.0f);
	// 			}
	// 			Rotate(pDC, PI / 4);


	// 			Rotate(pDC, PI / 4);
	// 			{
	// 				Scale(pDC, 0.7f, 1.0f);
	// 				pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 				Scale(pDC, 1.0f / 0.7f, 1.0f);

	// 				Translate(pDC, 0, -3 * cellSizeY);
	// 				{
	// 					// pDC->Ellipse(-r, -r, r, r);

	// 					// Scale(pDC, 0.5f, 1.0f);
	// 					pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 					// Scale(pDC, 2.0f, 1.0f);
	// 				}
	// 				Translate(pDC, 0, 3 * cellSizeY);
	// 			}
	// 			Rotate(pDC, -PI / 4);

	// 		}
	// 		Translate(pDC, 0, 3 * cellSizeY);

	// 	}
	// 	Rotate(pDC, PI / 4);

	// 	// Desna strana
	// 	Rotate(pDC, PI / 4);
	// 	{
	// 		Scale(pDC, 1.0f / 3.0f, 1.0f);
	// 		pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 		Scale(pDC, 3.0f, 1.0f);

	// 		Translate(pDC, 0, -3 * cellSizeY);
	// 		// pDC->Ellipse(-r, -r, r, r);

	// 		Rotate(pDC, -PI / 4 + angle2);
	// 		{
	// 			Scale(pDC, 0.7f, 1.0f);
	// 			pDC->PlayMetaFile(yellowPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 			Scale(pDC, 1.0f / 0.7f, 1.0f);
	// 		}
	// 		Rotate(pDC, PI / 4 - angle2);


	// 		Rotate(pDC, PI / 4);
	// 		{
	// 			Scale(pDC, 0.7f, 1.0f);
	// 			pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 			Scale(pDC, 1.0f / 0.7f, 1.0f);

	// 			Translate(pDC, 0, -3 * cellSizeY);
	// 			// pDC->Ellipse(-r, -r, r, r);

	// 			Rotate(pDC, -PI / 4);
	// 			{
	// 				Scale(pDC, 0.7f, 1.0f);
	// 				pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 				Scale(pDC, 1.0f / 0.7f, 1.0f);
	// 			}
	// 			Rotate(pDC, PI / 4);


	// 			Rotate(pDC, PI / 4);
	// 			{
	// 				Scale(pDC, 0.7f, 1.0f);
	// 				pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 				Scale(pDC, 1.0f / 0.7f, 1.0f);
	// 			}
	// 			Rotate(pDC, -PI / 4);
	// 		}
	// 		Rotate(pDC, -PI / 4);
	// 	}
	// 	Rotate(pDC, -PI / 4);
	// }

	// for (auto& junction : junctions) {
	// 	pDC->SetWorldTransform(&junction);
	// 	pDC->Ellipse(-r, -r, r, r);
	// }

	// pDC->SetWorldTransform(&oldTransform);











	// std::stack<Part> stack;

	// stack.push(p1);
	// while(!stack.empty()) {
	// 	auto &current = stack.top();
	// 	stack.pop();

	// 	Rotate(pDC, current.rotation);

	// 	if (current.controllable) {
	// 		pDC->PlayMetaFile(yellowPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 	} else {
	// 		pDC->PlayMetaFile(greenPart, CRect(-defaultHalfWidth, 0, defaultHalfWidth, -defaultHeight)); 
	// 	}

	// 	if (!current.children.empty()) {
	// 		for (auto p : current.children) {
	// 			stack.push(p);
	// 		}

	// 		Translate(pDC, 0, -defaultHeight);

	// 	}
	// }


	// return;