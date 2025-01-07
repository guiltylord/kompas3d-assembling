
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
	ON_WM_SIZE()
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
	DDX_Control(pDX, IDC_TEXT_CHOICE, m_sText);
	DDX_Control(pDX, IDC_GODMODE, m_btnGodMode);
	DDX_Control(pDX, IDC_EDIT_L, m_eL);
	DDX_Control(pDX, IDC_EDIT_R_HOLE, m_eRHole);
	DDX_Control(pDX, IDC_EDIT_R_BASE, m_eRBase);
	DDX_Control(pDX, IDC_TEXT_R_BASE, m_tRBase);
	DDX_Control(pDX, IDC_TEXT_R_HOLE, m_tRHole);
	DDX_Control(pDX, IDC_TEXT_L, m_tL);
}

void CAssemblingView::ConfigureWidgets(CString btnText, bool godMode)
{
	if (m_godMode) {
		//поменять текст но оставить комбушку
		//m_sText.ShowWindow(SW_HIDE); //на основе исполнения будет сделано:
		m_sText.SetWindowTextW(L"Based on THAT execution:");
		//m_cBox.ShowWindow(SW_HIDE);
		//m_cBox.MoveWindow(m_fieldL, m_imgH + 20, 125, 30);

		m_eL.ShowWindow(SW_NORMAL);
		m_eRBase.ShowWindow(SW_NORMAL);
		m_eRHole.ShowWindow(SW_NORMAL);
		m_tL.ShowWindow(SW_NORMAL);
		m_tRHole.ShowWindow(SW_NORMAL);
		m_tRBase.ShowWindow(SW_NORMAL);

		m_sText.MoveWindow(m_fieldL, m_imgH, 200, 30);
		m_cBox.MoveWindow(m_fieldL, m_imgH+30, 125, 30);

		m_tL.MoveWindow(m_fieldL, m_imgH+80, 125, 20);
		m_tRBase.MoveWindow(m_fieldL + 150, m_imgH+80, 125, 20);
		m_tRHole.MoveWindow(m_fieldL+300, m_imgH+80, 125, 20);
		m_eL.MoveWindow(m_fieldL, m_imgH+100, 125, 30);
		m_eRBase.MoveWindow(m_fieldL + 150, m_imgH+100, 125, 30);
		m_eRHole.MoveWindow(m_fieldL+300, m_imgH+100, 125, 30);
		m_btn.MoveWindow(m_fieldL, m_imgH+150, 125, 30);

		return;
	}
	m_sText.SetWindowTextW(L"Choose execution:");
	m_sText.ShowWindow(SW_NORMAL);
	m_cBox.ShowWindow(SW_NORMAL);
	m_btn.ShowWindow(SW_NORMAL);

	m_eL.ShowWindow(SW_HIDE);
	m_eRBase.ShowWindow(SW_HIDE);
	m_eRHole.ShowWindow(SW_HIDE);
	m_tL.ShowWindow(SW_HIDE);
	m_tRHole.ShowWindow(SW_HIDE);
	m_tRBase.ShowWindow(SW_HIDE);

	m_btnGodMode.ShowWindow(SW_HIDE);

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
	CAssemblingDoc* pDoc = GetDocument();
	switch (detail) {
	case ASSEMBLING: {
		ConfigurePicture(pDoc->imgPathForAssembling);
		ConfigureWidgets(pDoc->btnTextForAssembling, false);
		m_btnGodMode.ShowWindow(SW_NORMAL);
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
	case GOD: {
		ConfigurePicture(pDoc->imgPathForGod);
		ConfigureWidgets(pDoc->btnTextForAssembling, false);
		return;
	}
	}
	m_godMode = false;
}

bool CAssemblingView::CheckValues(CString RBase, CString RHole, CString L)
{
	if (L.IsEmpty() || RHole.IsEmpty() || RBase.IsEmpty()) {
		AfxMessageBox(L"Values cant be empty");
		return false;
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
	for (int i = 1; i <= 3; i++) {
		str.Format(L"%i", i);
		m_cBox.AddString(str);
	}
	m_cBox.SetCurSel(0);

	m_btnGodMode.ShowWindow(SW_HIDE);
	m_sText.ShowWindow(SW_HIDE);
	m_cBox.ShowWindow(SW_HIDE);
	m_btn.ShowWindow(SW_HIDE);
	m_eL.ShowWindow(SW_HIDE);
	m_eRBase.ShowWindow(SW_HIDE);
	m_eRHole.ShowWindow(SW_HIDE);
	m_tL.ShowWindow(SW_HIDE);
	m_tRHole.ShowWindow(SW_HIDE);
	m_tRBase.ShowWindow(SW_HIDE);
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
		if (m_godMode) {
			CString RBase;
			m_eRBase.GetWindowText(RBase);

			CString RHole;
			m_eRHole.GetWindowText(RHole);

			CString L;
			m_eL.GetWindowText(L);

			if (CheckValues(RBase, RHole, L)) {
				pDoc->m_pAssembler->GodAssemble(
					atoi(CT2A(RBase)),
					atoi(CT2A(RHole)),
					atoi(CT2A(L)));
			}
			return;							
		}									
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
	AfxMessageBox(L"You are now entering in creative mode. You can set more sizes but not all of them.");
	m_godMode = true;
	ConfigureWindow(GOD);
}


void CAssemblingView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if (!m_btnGodMode)
		return;

	if (cx <= 564 + 75) {
		m_btnGodMode.ShowWindow(SW_HIDE);
		return;
	}

	m_btnGodMode.ShowWindow(SW_NORMAL);
	m_btnGodMode.MoveWindow(cx-75, 0, 75, 30);
	// TODO: Add your message handler code here
}
