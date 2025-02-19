// CTreeDetails.cpp : implementation file
//

#include "pch.h"
#include "Assembling.h"
#include "CTreeDetails.h"
#include "AssemblingView.h"


// CTreeDetails

IMPLEMENT_DYNCREATE(CTreeDetails, CTreeView)

CTreeDetails::CTreeDetails()
{

}

CTreeDetails::~CTreeDetails()
{
}

BEGIN_MESSAGE_MAP(CTreeDetails, CTreeView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CTreeDetails diagnostics

#ifdef _DEBUG
void CTreeDetails::FillTree()
{
	CTreeCtrl& tree = GetTreeCtrl();

	tree.DeleteAllItems();

	m_hAssembling = tree.InsertItem(L"������", -1, -1, NULL, TVI_FIRST);

	m_hSeal = tree.InsertItem(L"������", -1, -1, m_hAssembling, TVI_ROOT);
	m_hScrew = tree.InsertItem(L"�������� �����", -1, -1, m_hAssembling, TVI_ROOT);
	m_hPuck = tree.InsertItem(L"�����", -1, -1, m_hAssembling, TVI_ROOT);

	tree.Expand(m_hAssembling, TVE_EXPAND);

	tree.SetCheck(m_hAssembling, m_pDoc->m_bAssembling);
	tree.SetCheck(m_hSeal, m_pDoc->m_bSeal);
	tree.SetCheck(m_hScrew, m_pDoc->m_bScrew);
	tree.SetCheck(m_hPuck, m_pDoc->m_bPuck);
}

void CTreeDetails::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CTreeDetails::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTreeDetails message handlers


int CTreeDetails::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= TVS_HASLINES
		| TVS_HASBUTTONS
		| TVS_LINESATROOT
		| TVS_SHOWSELALWAYS;

	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CTreeDetails::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTreeView::OnLButtonDown(nFlags, point);
	CTreeCtrl& tree = GetTreeCtrl();

	auto selTreeItem = tree.GetSelectedItem();
	if (selTreeItem == nullptr) {
		return;
	}

	if (selTreeItem==m_hAssembling) {
		m_pDoc->m_bAssembling = true;
		m_pDoc->m_bSeal = m_pDoc->m_bScrew = m_pDoc->m_bPuck = false;
		m_pDoc->m_pView->ConfigureWindow(ASSEMBLING);
	}
	if (selTreeItem==m_hSeal) {
		m_pDoc->m_bSeal = true; 
		m_pDoc->m_bAssembling = m_pDoc->m_bScrew = m_pDoc->m_bPuck  = false;
		m_pDoc->m_pView->ConfigureWindow(SEAL);

	}
	if (selTreeItem==m_hScrew) {
		m_pDoc->m_bScrew = true;
		m_pDoc->m_bAssembling = m_pDoc->m_bSeal =  m_pDoc->m_bPuck = false;
		m_pDoc->m_pView->ConfigureWindow(SCREW);
	}
	if (selTreeItem == m_hPuck) {
		m_pDoc->m_bPuck = true;
		m_pDoc->m_bAssembling = m_pDoc->m_bSeal = m_pDoc->m_bScrew = false;
		m_pDoc->m_pView->ConfigureWindow(PUCK);
	}
}
