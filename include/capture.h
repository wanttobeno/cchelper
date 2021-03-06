#pragma once

#include <windows.h>
#include <D3D9.h>
#include <D3dx9tex.h>
#include "Psapi.h"

class ScreenCapture
{
	unsigned int m_uWindowKey;
	bool	m_bSearchSubWindow;
	HWND	m_hwnd;
	DWORD	m_dwDisplayModeFormat;

public:

	DWORD GetDisplayModeFormat(){ return m_dwDisplayModeFormat; }

	HRESULT CaptureByD3D(HWND hWnd, LPCSTR szFileName);

	HRESULT CaptureByD3D(HWND hWnd, LPBYTE * pData);

	HWND SearchWindow(unsigned int uWindowKey, bool bSearchSubWindow = false);

	friend static BOOL CALLBACK ScreenCaptureSearch_EnumWindowsProc( HWND hwnd,LPARAM lParam);
	friend static BOOL CALLBACK ScreenCaptureSearch_EnumChildProc( HWND hwnd, LPARAM lParam);

	static unsigned int GetWindowKey(HWND hwnd);
};
