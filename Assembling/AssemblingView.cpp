
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
#include "Assembler.h"

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
	DDX_Control(pDX, ID_SKETCH1, m_sketchPic);
}

void CAssemblingView::ConfigurePictures(int detail)
{
	CImage img = CImage();

	switch (detail) {
	case SEAL: {
		img.Load(L"C:\\Users\\desxz\\source\\repos\\kompas3d-assembling\\kompas3d-assembling\\res\\1.bmp");
		break;
	}
	case SCREW: {
		break;
	}
	case PUCK: {
		break;
	}
	case ASSEMBLING: {
		img.Load(L"C:\\Users\\desxz\\Pictures\\Screenshots\\1.bmp");
		break;
	}
	}

	if (img == nullptr) {
		return;
	}

	HBITMAP bitmap = img.Detach();
	m_sketchPic.SetBitmap(bitmap);
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
	auto pDoc = GetDocument();
	if (pDoc->m_bSeal) {
		m_pAssembler->CreateSeal();
	}
	if (pDoc->m_bScrew) {
		m_pAssembler->CreateScrew();
	}
	if (pDoc->m_bPuck) {
		m_pAssembler->CreatePuck();
	}
	if (pDoc->m_bAssembling) {
		m_pAssembler->ass();
	}
}
