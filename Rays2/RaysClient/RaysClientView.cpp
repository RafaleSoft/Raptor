// RaysClientView.cpp : implementation of the CRaysClientView class
//

#include "stdafx.h"
#include "RaysClient.h"
#include "RaysClientDoc.h"
#include "RaysClientView.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRaysClientView

IMPLEMENT_DYNCREATE(CRaysClientView, CScrollView)

BEGIN_MESSAGE_MAP(CRaysClientView, CScrollView)
	//{{AFX_MSG_MAP(CRaysClientView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	// Other commands
	ON_COMMAND(ID_COMPILE,OnBuild)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRaysClientView construction/destruction

CRaysClientView::CRaysClientView()
	:orgx(0),orgy(0),
	Cursorx(0),Cursory(1),CursorOffset(0),CursorHeight(15),
	Font(NULL),DrawLine(-1)
{
}

CRaysClientView::~CRaysClientView()
{
}

BOOL CRaysClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = cs.style | WS_HSCROLL | WS_VSCROLL;

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRaysClientView drawing

void CRaysClientView::OnDraw(CDC* pDC)
{
	CRaysClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CBrush *myBrush = new CBrush(RGB(255,255,255));
	CRect myRect,myRect2;
	int Offset;

	if (Font!=NULL)
		pDC->SelectObject( Font );
	CSize mySize=pDoc->GetSize(pDC);
	this->SetScrollSizes(MM_TEXT,mySize);	
	this->HideCaret();
	this->GetClientRect( &myRect );

	Offset=orgy;
	for(int i=0;i<pDoc->NumLines();i++)
	{
		if (DrawLine<0)
		{
			if ((Offset>fontSpecs.lfHeight)&&(Offset<myRect.bottom))
				pDC->TextOut(orgx,Offset,pDoc->GetLine(i));
		}
		else if (DrawLine==i)
		{
/*			myRect2.left=0;myRect2.top=Offset;
			myRect2.right=myRect.right;
			myRect2.bottom=Offset+CursorHeight;
			pDC->FillRect(&myRect2,myBrush);*/
			pDC->TextOut(orgx,Offset,pDoc->GetLine(i));
		}

		Offset+=CursorHeight;
	}
	CPoint point(CursorOffset,orgy+Cursory*CursorHeight);

	this->SetCaretPos(point);	
	this->ShowCaret();
	delete(myBrush);

	this->ReleaseDC( pDC );
	DrawLine=-1;
}

/////////////////////////////////////////////////////////////////////////////
// CRaysClientView printing

BOOL CRaysClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRaysClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRaysClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRaysClientView diagnostics

#ifdef _DEBUG
void CRaysClientView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CRaysClientView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CRaysClientDoc* CRaysClientView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRaysClientDoc)));
	return (CRaysClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRaysClientView message handlers

void CRaysClientView::OnBuild()
{
	CString msg;
	CRaysClientDoc *pDoc = (CRaysClientDoc*)GetDocument();
	msg = "Compiling file: ";
	Cout(msg+pDoc->GetTitle());

	if (pDoc->Compile())
		Cout("Scene ready to render");
	else
		Cout("Errors found in text");
}

void CRaysClientView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
}

void CRaysClientView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CScrollView::OnMouseMove(nFlags, point);
}

void CRaysClientView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CScrollView::OnLButtonUp(nFlags, point);
}

int CRaysClientView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont *theFont=new CFont;
	fontSpecs.lfHeight = -13;
	fontSpecs.lfWidth = 0;
	fontSpecs.lfEscapement = 0;
	fontSpecs.lfOrientation = 0;
	fontSpecs.lfWeight = FW_NORMAL;
	fontSpecs.lfItalic = 0;
	fontSpecs.lfUnderline = 0;
	fontSpecs.lfStrikeOut = 0;
	fontSpecs.lfCharSet = 0;
	fontSpecs.lfOutPrecision = OUT_TT_PRECIS;
	fontSpecs.lfClipPrecision = CLIP_TT_ALWAYS;
	fontSpecs.lfQuality = PROOF_QUALITY;
	fontSpecs.lfPitchAndFamily = VARIABLE_PITCH;
	sprintf(fontSpecs.lfFaceName,"%s","Times New Roman");
	if (theFont->CreateFontIndirect(&fontSpecs))
		Font=theFont;
	else 
		Font = NULL;
	
	return 0;
}

void CRaysClientView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
	
	CPoint point(CursorOffset,orgy+Cursory*CursorHeight);
	this->CreateSolidCaret(CURSOR_WIDTH,CursorHeight);
	this->ShowCaret();
	this->SetCaretPos(point);
}

void CRaysClientView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
	
	this->HideCaret();
	::DestroyCaret();
}

void CRaysClientView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// If it's not me, check action requested
	if (pSender != this)
	{
		CRaysClientDoc *pDoc = GetDocument();
		pDoc->AddElement(lHint);
		//pDoc->SetModifiedFlag();
		pDoc->SetCompiled(false);
	}

	/*CDC *dc=this->GetDC();
	OnDraw(dc);
	this->ReleaseDC(dc);*/
}

void CRaysClientView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRaysClientDoc* pDoc = GetDocument();
	CDC *pDC = this->GetDC();
	CRect myRect;
	ASSERT_VALID(pDoc);

	if (Font!=NULL)
		pDC->SelectObject( Font );
	this->GetClientRect( &myRect );
	DrawLine=Cursory;

	switch(nChar)
	{
		case KEY_BKSPC:
			// Backspace sur une ligne non vide
			if(Cursorx>0)
			{
				pDoc->Extract(Cursory,Cursorx--);
				CSize mySize=pDoc->GetSize(pDC);

				// retracer si le caractere efface est en fin de 
				// la ligne la plus longue affichee
				if (mySize.cx+orgx<myRect.right-CURSOR_WIDTH)
					if (myRect.right-mySize.cx-CURSOR_WIDTH<0)
					{
						orgx=myRect.right-mySize.cx-CURSOR_WIDTH;
						DrawLine=-1;
					}
			}
			// Backspace qui efface une ligne
			else if (Cursory>0)
			{
				int Size=0;
				if ((Size=pDoc->GetLine(Cursory).GetLength())>0)
					pDoc->Expand(Cursory-1,pDoc->GetLine(Cursory));
				pDoc->DeleteLine(Cursory--);
				Cursorx=pDoc->GetLine(Cursory).GetLength()-Size;
				CSize mySize=pDoc->GetSize(pDC);

				// retracer le doc si la ligne effacee est 
				// en bas et laisse du vide
				if (mySize.cy+orgy<myRect.bottom)
					if (myRect.bottom-mySize.cy<0)
						orgy=myRect.bottom-mySize.cy;
				DrawLine=-1;
			}
			break;
		case KEY_TAB:
			{
				for (int i=0;i<TAB_SIZE;i++)
					pDoc->Insert(Cursory,Cursorx++,32);
			}
			break;
		case KEY_ENTER:
			{
				CString myString=pDoc->GetLine(Cursory);
				int Size=myString.GetLength();
				CString myString2=myString.Left(Cursorx);
				myString=myString.Right(Size-Cursorx);
				Cursorx=0;
				Cursory++;
				pDoc->NewLine(Cursory);
				pDoc->Expand(Cursory,myString);
				pDoc->SetLine(Cursory-1,myString2);
			}
			DrawLine=-1;
			break;
		default:
			pDoc->Insert(Cursory,Cursorx++,nChar);
	}

	CursorOffset=pDC->GetTextExtent((pDoc->GetLine(Cursory)).Left(Cursorx)).cx;
	this->ReleaseDC( pDC );		

	if (CursorOffset+orgx+CURSOR_WIDTH>myRect.right)
	{
		orgx=myRect.right-CursorOffset-CURSOR_WIDTH;
		DrawLine=-1;
	}
	else if (CursorOffset+orgx<0)
	{
		orgx=-CursorOffset;
		DrawLine=-1;
	}
	if (orgy+CursorHeight*Cursory+CursorHeight>myRect.bottom)
	{
		orgy=myRect.bottom-CursorHeight*Cursory-CursorHeight;
		DrawLine=-1;
	}
	else if (orgy+CursorHeight*Cursory<0)
	{
		orgy=-CursorHeight*Cursory;
		DrawLine=-1;
	}
	ScrollToPosition(CPoint(-orgx,-orgy));

	pDoc->SetModifiedFlag();
	pDoc->SetCompiled(false);

	//
	//	invalider uniquement la ligne modifiee
	//
	this->Invalidate();
	this->OnUpdate(this,0,NULL);

	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

void CRaysClientView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRaysClientDoc* pDoc = GetDocument();
	CDC *pDC = this->GetDC();
	
	CRect myRect;
	ASSERT_VALID(pDoc);
	BOOL Update=false,Move=false;

	DrawLine=Cursory;
	this->GetClientRect( &myRect );
	switch(nChar)
	{
		case KEY_DEL:
			// Delete sur une ligne non vide
			if (Cursorx<pDoc->GetLine(Cursory).GetLength())
			{
				if(Cursorx>0)
				{
					pDoc->Extract(Cursory,Cursorx+1);
					CSize mySize=pDoc->GetSize(pDC);
					this->GetClientRect( &myRect );
					if (mySize.cx+orgx<myRect.right-CURSOR_WIDTH)
						if (myRect.right-mySize.cx-CURSOR_WIDTH<0)
						{
							orgx=myRect.right-mySize.cx-CURSOR_WIDTH;
							DrawLine=-1;
						}
				}
			}
			// Delete qui efface une ligne
			else if (Cursory<pDoc->NumLines())
			{
				pDoc->Expand(Cursory,pDoc->GetLine(Cursory+1));
				pDoc->DeleteLine(Cursory+1);
				DrawLine=-1;
			}
			pDoc->SetModifiedFlag();
			pDoc->SetCompiled(false);
			Update=true;
			break;
		case KEY_LEFT:
			if (Cursorx>0)
				Cursorx--;
			else if (Cursory>0)
			{
				Cursory--;
				Cursorx=pDoc->GetLine(Cursory).GetLength();
			}
			Move=true;
			break;
		case KEY_RIGHT:
			if (Cursorx<pDoc->GetLine(Cursory).GetLength())
				Cursorx++;
			else if (Cursory<(pDoc->NumLines()-1))
			{
				Cursory++;
				Cursorx=0;
			}
			Move=true;
			break;
		case KEY_UP:
			if (Cursory>0)
			{
				int Size;
				Cursory--;
				if ((Size=pDoc->GetLine(Cursory).GetLength())<Cursorx)
					Cursorx=Size;
			}
			Move=true;
			break;
		case KEY_DOWN:
			if (Cursory<(pDoc->NumLines()-1))
			{
				int Size;
				Cursory++;
				if ((Size=pDoc->GetLine(Cursory).GetLength())<Cursorx)
					Cursorx=Size;
			}
			Move=true;
			break;
		case KEY_END:
			Cursorx=pDoc->GetLine(Cursory).GetLength();
			Move=true;
			break;
		case KEY_HOME:
			Cursorx=0;
			Move=true;
			break;
		case KEY_PGUP:
			{
				int Size=myRect.bottom/CursorHeight;
				if (Cursory-Size>=0)
					Cursory-=Size;
				else 
					Cursory=0;
			}
			DrawLine=-1;
			Move=true;
			break;
		case KEY_PGDOWN:
			{
				int Size=myRect.bottom/CursorHeight;
				if (Cursory+Size<pDoc->NumLines())
					Cursory+=Size;
				else
					Cursory=pDoc->NumLines()-1;
			}
			DrawLine=-1;
			Move=true;
			break;
	}

	if (Move)
	{
		if (Font!=NULL)
			pDC->SelectObject( Font );
		CursorOffset=pDC->GetTextExtent((pDoc->GetLine(Cursory)).Left(Cursorx)).cx;

		if (CursorOffset+orgx+CURSOR_WIDTH>myRect.right)
		{
			orgx=myRect.right-CursorOffset-CURSOR_WIDTH;
			Update=true;
		}
		else if (CursorOffset+orgx<0)
		{
			orgx=-CursorOffset;
			Update=true;
		}

		if (orgy+CursorHeight*Cursory+CursorHeight>myRect.bottom)
		{
			orgy=myRect.bottom-CursorHeight*Cursory-CursorHeight;	
			Update=true;
		}
		else if (orgy+CursorHeight*Cursory<0)
		{
			orgy=-CursorHeight*Cursory;
			Update=true;
		}

		DrawLine = -1;
		this->HideCaret();
		CPoint point(CursorOffset,orgy+Cursory*CursorHeight);
		this->SetCaretPos(point);	
		this->ShowCaret();
		ScrollToPosition(CPoint(-orgx,-orgy));	
	}

	if (Update)
	{
		this->Invalidate();
		this->OnUpdate(this,0,NULL);
	}
	
	this->ReleaseDC( pDC );
	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
