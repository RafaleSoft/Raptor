// RaysClientDoc.cpp : implementation of the CRaysClientDoc class
//

#include "stdafx.h"
#include "RaysClient.h"

#include "RaysClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRaysClientDoc

IMPLEMENT_DYNCREATE(CRaysClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CRaysClientDoc, CDocument)
	//{{AFX_MSG_MAP(CRaysClientDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRaysClientDoc construction/destruction

CRaysClientDoc::CRaysClientDoc():Compiled(false),nbObjects(0)
{
	Text.Add("*** Rays Meta Description File. ***");
	Text.Add("");
	SetModifiedFlag( FALSE );
}

CRaysClientDoc::~CRaysClientDoc()
{
	Text.RemoveAll();
	Text.FreeExtra();
}

BOOL CRaysClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

void CRaysClientDoc::PurgeObjects()
{
	for (int i=0;i<nbObjects;i++)
	{
		delete Objects[i];
		Objects[i] = NULL;
	}
	nbObjects=0;
}

/////////////////////////////////////////////////////////////////////////////
// CRaysClientDoc serialization

void CRaysClientDoc::Serialize(CArchive& ar)
{
	Text.FreeExtra();
	if (ar.IsStoring())
	{
		ar << Text.GetSize();
		for(int i=0;i<Text.GetSize();i++)
				ar << Text[i];
	}
	else
	{
		CString myString;
		int Size;

		Text.RemoveAll();
		ar >> Size;
		for (int i=0;i<Size;i++)
		{
			ar >> myString;
			Text.Add(myString);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRaysClientDoc diagnostics

#ifdef _DEBUG
void CRaysClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRaysClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRaysClientDoc commands
CString CRaysClientDoc::GenName(int ID)
{
	CString newName;
	newName.Format("%d",counter++);
	switch(ID)
	{
	case IDI_CAMAXES:
		newName="ACam"+newName;
		break;
	}
	return newName;
}

void CRaysClientDoc::AddElement(int ID)
{
	CString name;
	name.Format("%d",counter-1);

	switch(ID)
	{
	case IDI_CAMAXES:
		{
			Text.Add("AxisCamera ACam"+name);Text.Add("{");
			Text.Add("  Origin");
			Text.Add("  {");Text.Add("    X 0");Text.Add("    Y 0");Text.Add("    Z 0");Text.Add("  }");
			Text.Add("  Focale 0");
			Text.Add("  Aperture 0");
			Text.Add("  ObjectPlane 0");
			Text.Add("  Width 0");
			Text.Add("  Height 0");
			Text.Add("  Depth 0");
			Text.Add("  Variance 0");
			Text.Add("  ReflectionDepth 0");
			Text.Add("  RefractionDepth 0");
			Text.Add("}");
			Objects[nbObjects++] = new CCamera();
		}
		break;
	default:
		break;
	}
}


int	CRaysClientDoc::FindLine(CString subString,int fromLine)
{
	int num=fromLine;
	int max = Text.GetSize();
	while ((num<max)&&(0>Text[num].Find(subString)))
		num++;

	if (num==max) num = -1;
	return num;
}


const CString CRaysClientDoc::GetLine(int numLine) const
{
	_ASSERT((numLine>=0)&&(numLine<Text.GetSize()));
	return Text[numLine];
}

void CRaysClientDoc::SetLine(int numLine,CString newLine)
{
	_ASSERT((numLine>=0)&&(numLine<Text.GetSize()));
	Text.SetAt(numLine,newLine);
}

void CRaysClientDoc::NewLine(int numLine)
{
	_ASSERT((numLine>=0)&&(numLine<=Text.GetSize()));
	Text.InsertAt(numLine,"");
}

void CRaysClientDoc::DeleteLine(int numLine)
{
	_ASSERT((numLine>=0)&&(numLine<Text.GetSize()));
	Text.RemoveAt(numLine);
}

const CString CRaysClientDoc::MaxLine() const
{
	int MaxLength=0;
	CString Result;
	for(int i=0;i<Text.GetSize();i++)
	{
		if (Text[i].GetLength()>MaxLength)
		{
			Result=Text[i];
			MaxLength=Result.GetLength();
		}
	}
	return Result;
}

const CSize CRaysClientDoc::GetSize(CDC *pDC) const
{
	CSize Result(0,0);
	for(int i=0;i<Text.GetSize();i++)
	{
		CSize Size=pDC->GetTextExtent(Text[i]);
		if (Size.cx>Result.cx)
			Result.cx=Size.cx;
		Result.cy+=Size.cy;
	}
	return Result;
}


void CRaysClientDoc::Insert(int numLine,int numChar,char nChar)
{
	_ASSERT((numLine>=0)&&(numLine<Text.GetSize()));
	_ASSERT((numChar>=0)&&(numChar<=Text[numLine].GetLength()));
	// caractere en fin de chaine
	int Size=Text[numLine].GetLength();
	if (numChar==Size)
		Text[numLine]+=nChar;
	// insertion
	else
		Text[numLine]=Text[numLine].Left(numChar)+nChar+Text[numLine].Right(Size-numChar);
}

void CRaysClientDoc::Extract(int numLine,int numChar)
{
	_ASSERT((numLine>=0)&&(numLine<Text.GetSize()));
	int Size=Text[numLine].GetLength();
	_ASSERT((numChar>=0)&&(numChar<=Size));
	// caractere en fin de chaine
	if (numChar==Size)
		Text[numLine]=Text[numLine].Left(numChar-1);
	// extraction
	else
		Text[numLine]=Text[numLine].Left(numChar-1)+Text[numLine].Right(Size-numChar);
}

void CRaysClientDoc::Expand(int numLine,CString Tail)
{
	_ASSERT((numLine>=0)&&(numLine<Text.GetSize()));
	Text[numLine]+=Tail;
}

void CRaysClientDoc::AppendLine(CString newLine)
{
	Text.Add(newLine);
}


/////////////////////////////////////////////////////////////////////////////
// CRaysClientDoc data processing
#include "Parsing.cpp"



