// CTreeDetails.cpp : implementation file
//

#include "pch.h"
#include "Assembling.h"
#include "CTreeDetails.h"


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

	m_hClients = tree.InsertItem(L"Clients", -1, -1, NULL, TVI_ROOT);
	m_hOrders = tree.InsertItem(L"Orders", -1, -1, NULL, TVI_ROOT);
	m_hTours = tree.InsertItem(L"Tours", -1, -1, NULL, TVI_ROOT);

	tree.SetCheck(m_hClients, m_pDoc->m_bClients);
	tree.SetCheck(m_hOrders, m_pDoc->m_bOrders);
	tree.SetCheck(m_hTours, m_pDoc->m_bTours);
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
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CTreeDetails::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTreeView::OnLButtonDown(nFlags, point);
}
