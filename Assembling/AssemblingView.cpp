
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
	ON_WM_CTLCOLOR()
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
	DDX_Control(pDX, IDC_BUTTON1, btn_Build);
}

void CAssemblingView::ConfigureButton(CString btnText)
{
	auto width = GetSystemMetrics(SM_CXSCREEN);
	auto height = GetSystemMetrics(SM_CYSCREEN);

	btn_Build.MoveWindow(0, currImgHeight, 125, 30);
	btn_Build.SetWindowTextW(btnText);
}

void CAssemblingView::ConfigurePicture(CString imgPath)
{
	CImage img = CImage();

	img.Load(imgPath);

	if (img == nullptr) {
		currImgHeight = 0;
		return;
	}

	//Ширина картинки
	currImgHeight = img.GetHeight();

	HBITMAP bitmap = img.Detach();
	m_sketchPic.SetBitmap(bitmap);
}

void CAssemblingView::ConfigureWindow(const int detail)
{
	CAssemblingDoc* pDoc = GetDocument();
	switch (detail) {
	case ASSEMBLING: {
		ConfigurePicture(pDoc->imgPathForAssembling);
		ConfigureButton(pDoc->btnTextForAssembling);
		break;
	}
	case SEAL: {
		ConfigurePicture(pDoc->imgPathForSeal);
		ConfigureButton(pDoc->btnTextForSeal);
		break;
	}
	case SCREW: {
		ConfigurePicture(pDoc->imgPathForScrew);
		ConfigureButton(pDoc->btnTextForScrew);
		break;
	}
	case PUCK: {
		ConfigurePicture(pDoc->imgPathForPuck);
		ConfigureButton(pDoc->btnTextForPuck);
		break;
	}
	}
}

//void CAssemblingView::ConfigureWindow()
//{
//
//	if()
//
//}



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


HBRUSH CAssemblingView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}
