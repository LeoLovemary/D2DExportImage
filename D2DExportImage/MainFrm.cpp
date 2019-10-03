
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "D2DExportImage.h"
#include "D2DExportImageDoc.h"
#include "D2DExportImageView.h"
#include "MainFrm.h"
#include <filesystem>

namespace fs = std::filesystem;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLS_CONVERT, &CMainFrame::OnToolsConvert)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers



void CMainFrame::OnToolsConvert()
{
	D2D1_SIZE_F imagesize;
	HRESULT hr = S_OK;
	wchar_t ownPth[MAX_PATH];
	
	CD2DExportImageView* pView = static_cast<CD2DExportImageView*> (this->GetActiveView());
	

	if (pView->GetFactory() == nullptr)
	{
		if (FAILED(pView->Init2D2BitmapFactory()))
		{
			return;
		}
	}

	if (GetModuleFileName(GetModuleHandle(NULL), ownPth, (sizeof(ownPth))) == 0)
	{
		return;
	}
	

	fs::path path_exe = ownPth;


	CFolderPickerDialog folderPickerDialog(path_exe.parent_path().native().c_str(), OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, this,
		sizeof(OPENFILENAME));

	CString folderPath;

	if (folderPickerDialog.DoModal() == IDOK)
	{

		POSITION pos = folderPickerDialog.GetStartPosition();

		while (pos)
		{
			folderPath = folderPickerDialog.GetNextPathName(pos);
		}
	}


	if (folderPath != _T(""))
	{

		std::wstring path = folderPath;

		
		for (const auto& entry : fs::directory_iterator(path))
		{
			fs::path current_file = entry.path();


			if (!fs::is_directory(current_file)) //we eliminate directories
			{				
				OutputDebugString(current_file.native().c_str());
				if (current_file.extension() == L".svg")
				{

					fs::path file_out = path;
					file_out.append(current_file.filename().c_str());

					hr = pView->GetSvgDocumentSize(current_file.native().c_str(), imagesize);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("02.png"), imagesize, 2.0f);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("04.png"), imagesize, 4.0f);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("08.png"), imagesize, 8.0f);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("05.png"), imagesize, 0.5f);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("03.png"), imagesize, 0.3f);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("0.2.png"), imagesize, 0.2f);
					pView->SaveImage(current_file.native().c_str(), file_out += _T("0.1.png"), imagesize, 0.1f);
				}
			}
		}
	}
}

