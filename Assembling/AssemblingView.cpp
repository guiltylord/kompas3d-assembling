
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
	ON_COMMAND(ID_KOMPAS_CLOSEALL, &CAssemblingView::OnKompasCloseall)
	ON_BN_CLICKED(IDC_GODMODE, &CAssemblingView::OnBnClickedGodmode)
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
	DDX_Control(pDX, IDC_BUTTON1, m_btn);
	DDX_Control(pDX, IDC_COMBOBOX, m_cBox);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_sText);
	DDX_Control(pDX, IDC_GODMODE, m_btnGodMode);
	DDX_Control(pDX, IDC_EDIT_L, m_eL);
	DDX_Control(pDX, IDC_EDIT_RAD_HOLE, m_eRHole);
	DDX_Control(pDX, IDC_EDIT_RAD_BASE, m_eRBase);
}

void CAssemblingView::ConfigureWidgets(CString btnText, bool godMode)
{
	if (m_godMode) {
		m_eL.ShowWindow(SW_NORMAL);
		m_eRBase.ShowWindow(SW_NORMAL);
		m_eRHole.ShowWindow(SW_NORMAL);

		m_eL.MoveWindow(m_fieldL, m_imgH+30, 125, 30);
		m_eRBase.MoveWindow(m_fieldL + 150, m_imgH+30, 125, 30);
		m_eRHole.MoveWindow(m_fieldL+300, m_imgH+30, 125, 30);
		m_btn.MoveWindow(m_fieldL, m_imgH+80, 125, 30);

		return;
	}
	m_sText.MoveWindow(m_fieldL, m_imgH, 125, 20);
	m_cBox.MoveWindow(m_fieldL, m_imgH + 20, 125, 30);
	m_btn.MoveWindow(m_fieldL, m_imgH+50, 125, 30);
	m_btn.SetWindowTextW(btnText);
}

void CAssemblingView::ConfigurePicture(CString imgPath)
{
	CImage img = CImage();
	img.Load(imgPath);
	if (img == nullptr) {
		m_imgH = 0;
		return;
	}

	m_imgH = img.GetHeight();

	HBITMAP bitmap = img.Detach();
	m_sketchPic.SetBitmap(bitmap);
}

void CAssemblingView::ConfigureWindow(const int detail)
{
	m_godMode = false;
	CAssemblingDoc* pDoc = GetDocument();
	m_sText.ShowWindow(SW_NORMAL);
	m_cBox.ShowWindow(SW_NORMAL);
	m_btn.ShowWindow(SW_NORMAL);
	m_btnGodMode.ShowWindow(SW_HIDE);
	m_eL.ShowWindow(SW_HIDE);
	m_eRBase.ShowWindow(SW_HIDE);
	m_eRHole.ShowWindow(SW_HIDE);
	switch (detail) {
	case ASSEMBLING: {
		m_btnGodMode.ShowWindow(SW_NORMAL);
		ConfigurePicture(pDoc->imgPathForAssembling);
		ConfigureWidgets(pDoc->btnTextForAssembling, false);
		break;
	}
	case SEAL: {
		ConfigurePicture(pDoc->imgPathForSeal);
		ConfigureWidgets(pDoc->btnTextForSeal, false);
		break;
	}
	case SCREW: {
		ConfigurePicture(pDoc->imgPathForScrew);
		ConfigureWidgets(pDoc->btnTextForScrew, false);
		break;
	}
	case PUCK: {
		ConfigurePicture(pDoc->imgPathForPuck);
		ConfigureWidgets(pDoc->btnTextForPuck, false);
		break;
	}
	}
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

	CString str;
	for (int i = 1; i <= 4; i++) {
		str.Format(L"%i", i);
		m_cBox.AddString(str);
	}
	m_cBox.SetCurSel(0);
	//m_sText.back
	m_sText.ShowWindow(SW_HIDE);
	m_cBox.ShowWindow(SW_HIDE);
	m_btn.ShowWindow(SW_HIDE);
	m_btnGodMode.ShowWindow(SW_HIDE);
	m_eL.ShowWindow(SW_HIDE);
	m_eRBase.ShowWindow(SW_HIDE);
	m_eRHole.ShowWindow(SW_HIDE);
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
	int execution = m_cBox.GetCurSel()+1;
	auto pDoc = GetDocument();
	if (pDoc->m_bSeal) {
		pDoc->m_pAssembler->FillAssembler(execution);
		pDoc->m_pAssembler->CreateSeal();
	}
	if (pDoc->m_bScrew) {
		pDoc->m_pAssembler->FillAssembler(execution);
		pDoc->m_pAssembler->CreateScrew();
	}
	if (pDoc->m_bPuck) {
		pDoc->m_pAssembler->FillAssembler(execution);
		pDoc->m_pAssembler->CreatePuck();
	}
	if (pDoc->m_bAssembling) {
		pDoc->m_pAssembler->FillAssembler(execution);
		pDoc->m_pAssembler->MakeAssemble();
	}
}


HBRUSH CAssemblingView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetTextColor(RGB(255, 0, 0));
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}


void CAssemblingView::OnKompasCloseall()
{
	auto pDoc = GetDocument();
	pDoc->m_pAssembler->CloseAll();
}


void CAssemblingView::OnBnClickedGodmode()
{
	m_sText.ShowWindow(SW_HIDE);
	m_cBox.ShowWindow(SW_HIDE);
	m_godMode = true;
	CAssemblingDoc* pDoc = GetDocument();

	ConfigurePicture(pDoc->imgPathForGod);
	ConfigureWidgets(pDoc->btnTextForAssembling, false);
	// TODO: Add your control notification handler code here
}
