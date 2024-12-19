
// AssemblingView.h : interface of the CAssemblingView class
//

#pragma once

#include "AssemblingDoc.h"
#include "CTreeDetails.h"

class CAssemblingView : public CFormView
{
protected: // create from serialization only
	CAssemblingView() noexcept;
	DECLARE_DYNCREATE(CAssemblingView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_ASSEMBLING_FORM };
#endif

// Attributes
public:
	CAssemblingDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CAssemblingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

#ifndef _DEBUG  // debug version in AssemblingView.cpp
inline CAssemblingDoc* CAssemblingView::GetDocument() const
   { return reinterpret_cast<CAssemblingDoc*>(m_pDocument); }
#endif

