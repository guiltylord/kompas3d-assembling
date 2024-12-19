#pragma once

#include "AssemblingDoc.h"
// CTreeDetails view

class CTreeDetails : public CTreeView
{
	DECLARE_DYNCREATE(CTreeDetails)

protected:
	CTreeDetails();           // protected constructor used by dynamic creation
	virtual ~CTreeDetails();

public:
	CAssemblingDoc* m_pDoc;
	HTREEITEM m_hAssembling, m_hSeal, m_hScrew, m_hPuck;

	void FillTree();
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


