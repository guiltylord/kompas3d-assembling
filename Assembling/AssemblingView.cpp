
// AssemblingView.cpp : implementation of the CAssemblingView class
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

#include "C:\Program Files\ASCON\KOMPAS-3D v22 Study\SDK\Include\ksConstants.h"
#include "C:\Program Files\ASCON\KOMPAS-3D v22 Study\SDK\Include\ksConstants3D.h"

#import "C:\Program Files\ASCON\KOMPAS-3D v22 Study\SDK\lib\kAPI5.tlb"

using namespace Kompas6API5;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAssemblingView

IMPLEMENT_DYNCREATE(CAssemblingView, CFormView)

BEGIN_MESSAGE_MAP(CAssemblingView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CAssemblingView::OnBnClickedButton1)
END_MESSAGE_MAP()

// CAssemblingView construction/destruction

CAssemblingView::CAssemblingView() noexcept
	: CFormView(IDD_ASSEMBLING_FORM)
{
	// TODO: add construction code here

}

CAssemblingView::~CAssemblingView()
{
}

void CAssemblingView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CAssemblingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CAssemblingView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CAssemblingView diagnostics

#ifdef _DEBUG
void CAssemblingView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAssemblingView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAssemblingDoc* CAssemblingView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAssemblingDoc)));
	return (CAssemblingDoc*)m_pDocument;
}
#endif //_DEBUG


// CAssemblingView message handlers

void CAssemblingView::OnBnClickedButton1()
{
	KompasObjectPtr kompas;

	kompas.CreateInstance(L"Kompas.Application.5");
	kompas->Visible = true;

	ksDocument3DPtr doc;
	doc = kompas->Document3D();
	doc->Create(false, true);
	doc = kompas->ActiveDocument3D();
	ksPartPtr part = doc->GetPart(pTop_Part);

	//эскиз под вращение:
	ksEntityPtr sketch = part->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDef = sketch->GetDefinition();
	sketchDef->SetPlane(part->GetDefaultEntity(o3d_planeXOY));
	sketch->Create();
	ksDocument2DPtr doc2D = sketchDef->BeginEdit();
	doc2D->ksLineSeg(0, 0, 0, 10, 1);
	sketchDef->EndEdit();

	//операция сохранения детали
	CString name = L"Штифт";
	doc->fileName = _bstr_t(name);
	doc->SaveAs(L"C:\\Users\\desxz\\source\\repos\\Assembling\\Assembling");
}
