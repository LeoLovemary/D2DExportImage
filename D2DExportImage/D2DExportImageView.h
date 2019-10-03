
// D2DExportImageView.h : interface of the CD2DExportImageView class
//

#pragma once


class CD2DExportImageView : public CView
{
protected: // create from serialization only
	CD2DExportImageView() noexcept;
	DECLARE_DYNCREATE(CD2DExportImageView)

// Attributes
public:
	CD2DExportImageDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CD2DExportImageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	HRESULT Init2D2BitmapFactory();
	HRESULT GetSvgDocumentSize(std::wstring str_sourcefile, D2D1_SIZE_F& imagesize);
	void SaveImage(std::wstring str_sourcefile, std::wstring str_destfile, D2D1_SIZE_F imagesize, FLOAT factor);
	CComPtr<ID2D1Factory> GetFactory() { return pD2DFactory_; }
private:
	HRESULT DrawImage(CComPtr <ID2D1RenderTarget> pRT, std::wstring str_sourcefile, D2D1_SIZE_F imagesize, FLOAT factor);
	void ExportImage(std::wstring str_destfile, D2D1_SIZE_F imagesize, FLOAT factor);


private:
	CComPtr<ID2D1Factory>		pD2DFactory_ = nullptr;
	CComPtr <IWICBitmap>		pWICBitmap_ = nullptr;
	CComPtr <IWICImagingFactory> pWICFactory_ = nullptr;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnAfxDraw2D(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAfxRecreated2DResources(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in D2DExportImageView.cpp
inline CD2DExportImageDoc* CD2DExportImageView::GetDocument() const
   { return reinterpret_cast<CD2DExportImageDoc*>(m_pDocument); }
#endif

