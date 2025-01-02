
// AssemblingDoc.cpp : implementation of the CAssemblingDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Assembling.h"
#endif

#include "AssemblingDoc.h"
#include "AssemblingView.h"
#include "CTreeDetails.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Assembler.h"

// CAssemblingDoc

IMPLEMENT_DYNCREATE(CAssemblingDoc, CDocument)

BEGIN_MESSAGE_MAP(CAssemblingDoc, CDocument)
END_MESSAGE_MAP()

// CAssemblingDoc construction/destruction
class Assembler;
class CAssemblingView;
CAssemblingDoc::CAssemblingDoc() noexcept
{
	// TODO: add one-time construction code here

}

CAssemblingDoc::~CAssemblingDoc()
{
}

void CAssemblingDoc::GetPaths()
{
	//evaluating paths to images in curr dir  or OTHER OPTION
	int i;
}

BOOL CAssemblingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	m_bAssembling = m_bSeal = m_bScrew = m_bPuck = false;
	
	GetPaths();
	m_pTree->m_pDoc = this;
	m_pTree->FillTree();
	m_pView->m_pAssembler = new Assembler();

	return TRUE;
}




// CAssemblingDoc serialization

void CAssemblingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CAssemblingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CAssemblingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CAssemblingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CAssemblingDoc diagnostics

#ifdef _DEBUG
void CAssemblingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAssemblingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAssemblingDoc commands
