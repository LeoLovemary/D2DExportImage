
// D2DExportImage.h : main header file for the D2DExportImage application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CD2DExportImageApp:
// See D2DExportImage.cpp for the implementation of this class
//

class CD2DExportImageApp : public CWinApp
{
public:
	CD2DExportImageApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CD2DExportImageApp theApp;
