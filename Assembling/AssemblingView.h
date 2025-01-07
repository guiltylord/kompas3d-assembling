
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
#define GOD 999




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
	//Assembler* m_pAssembler;
	int m_imgH;
	int m_fieldL = 20;
	bool m_godMode = false;

// Operations
public:
	void ConfigureWidgets(CString btnText, bool godMode);
	void ConfigurePicture(CString imgPath);
	void ConfigureWindow(const int detail);
	bool CheckValues(CString RBase, CString RHole, CString L);

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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CStatic m_sketchPic;
	CButton m_btn;
	CComboBox m_cBox;
	CStatic m_sText;
	afx_msg void OnKompasCloseall();
	CButton m_btnGodMode;
	afx_msg void OnBnClickedGodmode();
	CEdit m_eL;
	CEdit m_eRHole;
	CEdit m_eRBase;
	CStatic m_tRBase;
	CStatic m_tRHole;
	CStatic m_tL;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in AssemblingView.cpp
inline CAssemblingDoc* CAssemblingView::GetDocument() const
   { return reinterpret_cast<CAssemblingDoc*>(m_pDocument); }
#endif

