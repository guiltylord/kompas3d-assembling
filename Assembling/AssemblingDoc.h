
// AssemblingDoc.h : interface of the CAssemblingDoc class
//


#pragma once
#include <filesystem>
#include <string>
#include "Assembler.h"

class CTreeDetails;
class CAssemblingView;

class CAssemblingDoc : public CDocument
{
protected: // create from serialization only
	CAssemblingDoc() noexcept;
	DECLARE_DYNCREATE(CAssemblingDoc)

// Attributes
public:
	Assembler* m_pAssembler;

	bool m_bAssembling, m_bSeal, m_bScrew, m_bPuck;

	std::string m_cwd;
	CString btnTextForAssembling = L"Made assembling";
	CString btnTextForSeal = L"Build seal";
	CString btnTextForScrew = L"Build screw";
	CString btnTextForPuck = L"Build puck";

	CString imgPathForGod;
	CString imgPathForAssembling;
	CString imgPathForSeal;
	CString imgPathForScrew;
	CString imgPathForPuck;

	CTreeDetails* m_pTree;
	CAssemblingView* m_pView;
// Operations
public:
	void GetPaths();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CAssemblingDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
