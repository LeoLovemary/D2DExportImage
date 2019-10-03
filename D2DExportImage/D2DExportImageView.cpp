
// D2DExportImageView.cpp : implementation of the CD2DExportImageView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "D2DExportImage.h"
#endif

#include "D2DExportImageDoc.h"
#include "D2DExportImageView.h"
#include <d2d1_3.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CD2DExportImageView

IMPLEMENT_DYNCREATE(CD2DExportImageView, CView)

BEGIN_MESSAGE_MAP(CD2DExportImageView, CView)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, OnAfxDraw2D)
	ON_REGISTERED_MESSAGE(AFX_WM_RECREATED2DRESOURCES, OnAfxRecreated2DResources)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CD2DExportImageView construction/destruction

CD2DExportImageView::CD2DExportImageView() noexcept
{
	// TODO: add construction code here

}

CD2DExportImageView::~CD2DExportImageView()
{
}

BOOL CD2DExportImageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

int CD2DExportImageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableD2DSupport(); // Enable Direct2D support for this window:


	return 0;
}

// CD2DExportImageView drawing

void CD2DExportImageView::OnDraw(CDC* /*pDC*/)
{
	CD2DExportImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

LRESULT CD2DExportImageView::OnAfxRecreated2DResources(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}

LRESULT CD2DExportImageView::OnAfxDraw2D(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	CHwndRenderTarget* pRenderTarget = reinterpret_cast<CHwndRenderTarget*>(lParam);

	Invalidate();

	return TRUE;
}

// CD2DExportImageView printing

BOOL CD2DExportImageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CD2DExportImageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CD2DExportImageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

HRESULT CD2DExportImageView::Init2D2BitmapFactory()
{
	HRESULT hr = S_OK;

	
	if (FAILED(hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&pWICFactory_))))
	{
		OutputDebugString(L"CoCreateInstance CLSID_WICImagingFactory failed");
		return hr;
	}


	if (FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory_)))
	{
		OutputDebugString(L"D2D1CreateFactory failed");
		return hr;
	}
	return (hr);
}

HRESULT CD2DExportImageView::GetSvgDocumentSize(std::wstring str_sourcefile, D2D1_SIZE_F& imagesize)
{
	HRESULT						hr			= S_OK;
	CComPtr<ID2D1SvgDocument>	svgDocument = nullptr;
	CComPtr<IStream>			pstream		= nullptr;
	CComQIPtr<ID2D1SvgElement>	svgelement	= nullptr;
	CComPtr <ID2D1RenderTarget> pRT			= nullptr;
	CComPtr <IStream>			ptemplate	= nullptr;
	CComPtr <IWICBitmap>		pWICBitmap	= nullptr;
	FLOAT						tmpheight	= 0;
	FLOAT						tmpwidth	= 0;
	D2D1_SIZE_F					s			= {100,100};

	if (FAILED(hr = pWICFactory_->CreateBitmap( static_cast<UINT>(s.height), static_cast<UINT>(s.width), GUID_WICPixelFormat32bppBGR, WICBitmapCacheOnLoad, &pWICBitmap)))
	{
		OutputDebugString(L"pWICFactory failed");
		return (hr);
	}

	if (FAILED(hr = pD2DFactory_->CreateWicBitmapRenderTarget(pWICBitmap, D2D1::RenderTargetProperties(), &pRT)))
	{
		OutputDebugString(L"pD2DFactory failed");
		return (hr);
	}

	if (FAILED(hr = SHCreateStreamOnFileEx(str_sourcefile.c_str(), STGM_READ, 0, FALSE, ptemplate, &pstream)))
	{
		ASSERT(S_OK != hr);
		return (hr);
	}

	CComQIPtr<ID2D1DeviceContext5> spDeviceContext5 = pRT;

	if (FAILED(hr = spDeviceContext5->CreateSvgDocument(pstream, s, &svgDocument)))
	{
		ASSERT(S_OK != hr);
		return hr;
	}

	svgDocument->GetRoot(&svgelement);

	D2D1_SVG_VIEWBOX viewBox;

	if (FAILED(hr = svgelement->GetAttributeValue(L"viewBox", D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX, static_cast<void*>(&viewBox), sizeof(viewBox))))
	{
		viewBox.height = viewBox.width = 0;
	}

	if (FAILED(hr = svgelement->GetAttributeValue(L"width", &tmpwidth)))
	{		
		tmpwidth = 0;
	}

	if (FAILED(hr = svgelement->GetAttributeValue(L"height", &tmpheight)))
	{
	
		tmpheight = 0;
	}

	if (viewBox.height != 0 && viewBox.width != 0)
	{
		imagesize.height = viewBox.height;
		imagesize.width = viewBox.width;
	}

	if (tmpheight != 0 && tmpwidth != 0)
	{
		imagesize.height = tmpheight;
		imagesize.width = tmpwidth;
	}

	if (imagesize.width == 0 && imagesize.height == 0)
	{
		return E_FAIL;
	}

	return hr;
}

HRESULT CD2DExportImageView::DrawImage(CComPtr <ID2D1RenderTarget> pRT, std::wstring str_sourcefile, D2D1_SIZE_F imagesize, FLOAT factor)
{
	IStream* pstream = nullptr;
	IStream* ptemplate = NULL;
	HRESULT hr = S_OK;

	if (FAILED(hr = SHCreateStreamOnFileEx(str_sourcefile.c_str(), STGM_READ, 0, FALSE, ptemplate, &pstream)))
	{
		ASSERT(S_OK != hr);
		return hr;
	}

	CComQIPtr<ID2D1DeviceContext5> spDeviceContext = pRT;

	D2D1_SIZE_F s = {imagesize.height, imagesize.width  };
	CComPtr<ID2D1SvgDocument> svgDocument;

	if (FAILED(hr = spDeviceContext->CreateSvgDocument(pstream, s, &svgDocument)))
	{
		ASSERT(S_OK != hr);
		return hr;
	}





	//FLOAT radio = (svg_Height * 100) / (FLOAT) size.cx;

	D2D1_SIZE_F viewportsize = svgDocument->GetViewportSize();
	auto transform = D2D1::Matrix3x2F::Scale(D2D1::SizeF(factor, factor), D2D1::Point2F(0, 0));
	spDeviceContext->SetTransform(transform);

	if (svgDocument == nullptr)
	{
		ASSERT(S_OK != hr);
		return hr;
	}
	spDeviceContext->DrawSvgDocument(svgDocument);
	spDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	return (hr);
}

void CD2DExportImageView::SaveImage(std::wstring str_sourcefile, std::wstring str_destfile, D2D1_SIZE_F imagesize, FLOAT factor)
{
	HRESULT hr = S_OK;
	CComPtr <ID2D1RenderTarget> pRT = nullptr;
	
	if (pWICBitmap_ != nullptr)
	{
		pWICBitmap_ = nullptr;
	}
	
	if (FAILED(hr = pWICFactory_->CreateBitmap(static_cast<UINT>(imagesize.width * factor), static_cast<UINT>(imagesize.height * factor), GUID_WICPixelFormat32bppBGR, WICBitmapCacheOnLoad, &pWICBitmap_)))
	{
		OutputDebugString(L"pWICFactory failed");
		return;
	}

	if (FAILED(hr = pD2DFactory_->CreateWicBitmapRenderTarget(pWICBitmap_, D2D1::RenderTargetProperties(), &pRT)))
	{
		OutputDebugString(L"pD2DFactory failed");
		return;
	}

	pRT->BeginDraw();
	pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
	DrawImage(pRT, str_sourcefile, imagesize, factor);
	hr = pRT->EndDraw();
	ExportImage(str_destfile,imagesize, factor);
}

void CD2DExportImageView::ExportImage(std::wstring str_destfile, D2D1_SIZE_F imagesize, FLOAT factor)
{
	HRESULT hr;
	CComPtr <IWICStream> pStream = nullptr;
	CComPtr <IWICBitmapEncoder> pEncoder = nullptr;
	CComPtr <IWICBitmapFrameEncode> pFrameEncode = nullptr;

	if (FAILED(hr = pWICFactory_->CreateStream(&pStream)))
	{
		OutputDebugString(L"CreateStream failed");
		return;
	}

	WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;

	if (FAILED(hr = pStream->InitializeFromFilename(str_destfile.c_str(), GENERIC_WRITE)))
	{
		OutputDebugString(L"InitializeFromFilename failed");
		return;
	}
	if (FAILED(hr = pWICFactory_->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder)))
	{
		OutputDebugString(L"CreateEncoder failed");
		return;
	}
	if (FAILED(hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache)))
	{
		OutputDebugString(L"Initialize failed");
		return;
	}
	if (FAILED(hr = pEncoder->CreateNewFrame(&pFrameEncode, NULL)))
	{
		OutputDebugString(L"CreateNewFrame failed");
		return;
	}
	if (FAILED(hr = pFrameEncode->Initialize(NULL)))
	{
		OutputDebugString(L"Initialize failed");
		return;
	}
	if (FAILED(hr = pFrameEncode->SetSize(static_cast<UINT>(imagesize.width * factor), static_cast<UINT>(imagesize.height * factor))))
	{
		OutputDebugString(L"SetSize failed");
		return;
	}
	if (FAILED(hr = pFrameEncode->SetPixelFormat(&format)))
	{
		OutputDebugString(L"SetPixelFormat failed");
		return;
	}
	if (FAILED(hr = pFrameEncode->WriteSource(pWICBitmap_, NULL)))
	{
		OutputDebugString(L"WriteSource failed");
		return;
	}
	if (FAILED(hr = pFrameEncode->Commit()))
	{
		OutputDebugString(L"Commit failed");
		return;
	}
	if (FAILED(hr = pEncoder->Commit()))
	{
		OutputDebugString(L"Commit failed");
		return;
	}

}

// CD2DExportImageView diagnostics

#ifdef _DEBUG
void CD2DExportImageView::AssertValid() const
{
	CView::AssertValid();
}

void CD2DExportImageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CD2DExportImageDoc* CD2DExportImageView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CD2DExportImageDoc)));
	return (CD2DExportImageDoc*)m_pDocument;
}
#endif //_DEBUG


// CD2DExportImageView message handlers
