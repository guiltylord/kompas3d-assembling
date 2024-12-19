
// AssemblingDoc.h : interface of the CAssemblingDoc class
//



#pragma once

class CTreeDetails;
class CAssemblingView;

class CAssemblingDoc : public CDocument
{
protected: // create from serialization only
	CAssemblingDoc() noexcept;
	DECLARE_DYNCREATE(CAssemblingDoc)

// Attributes
public:
	bool m_bAssembling, m_bSeal, m_bScrew, m_bPuck;

	CTreeDetails* m_pTree;
	CAssemblingView* m_pView;
// Operations
public:

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