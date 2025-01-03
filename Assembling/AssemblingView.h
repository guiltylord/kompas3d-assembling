
// AssemblingView.h : interface of the CAssemblingView class
//

#pragma once

#include "AssemblingDoc.h"
#include "Assembler.h"
#include "CTreeDetails.h"

#define SEAL 1 
#define SCREW 2 
#define PUCK 3 
#define ASSEMBLING 0




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
	Assembler* m_pAssembler;
	int currImgHeight;

// Operations
public:
	void ConfigureButton(CString btnText);
	void ConfigurePicture(CString imgPath);
	void ConfigureWindow(const int detail);

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
	CStatic m_sketchPic;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton btn_Build;
};

#ifndef _DEBUG  // debug version in AssemblingView.cpp
inline CAssemblingDoc* CAssemblingView::GetDocument() const
   { return reinterpret_cast<CAssemblingDoc*>(m_pDocument); }
#endif

