// mousehook.cpp : Defines the exported functions for the DLL application.
//

#include <windows.h>
#include "mousehook.h"
#include "log.h"
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#pragma data_seg("TWH.ShareData")
HWND	CMouseHook::m_hWnd	= NULL;

#pragma data_seg()
#pragma comment(linker, "/section:TWH.ShareData,rws")

HMODULE CMouseHook::m_hModule = NULL;
HHOOK   CMouseHook::m_hHook	= NULL;
UINT	CMouseHook::UWM_DRAGEEND = NULL;


void ErrorMsg()
{
	LPVOID lpMsgBuf;

	FormatMessage(

		FORMAT_MESSAGE_ALLOCATE_BUFFER |

		FORMAT_MESSAGE_FROM_SYSTEM |

		FORMAT_MESSAGE_IGNORE_INSERTS,

		NULL,

		GetLastError(),

		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), // 既定の言語

		(LPTSTR) &lpMsgBuf,

		0,

		NULL

	);

	// lpMsgBuf 内のすべての挿入シーケンスを処理する。

	// ...

	// 文字列を表示する。

	base::Log(9, "Error:%s", lpMsgBuf );
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);

	// バッファを解放する。

	LocalFree(lpMsgBuf);

}

LRESULT CALLBACK CMouseHook::GetMsgProc(int nCode,
    WPARAM wParam,
    LPARAM lParam
)
{
	LPMSG msg = (LPMSG)lParam;

	if( m_hWnd )
	{
		if(msg->message  == WM_LBUTTONUP)
		{
			//char buf[255];
			//_stprintf(buf, "CMouseHook::UWM_DRAGEEND=%x",CMouseHook::UWM_DRAGEEND);
			//OutputDebugString(buf);
			POINT pt;
			GetCursorPos(&pt);
			HWND hwnd = WindowFromPoint(pt);
			if( m_hWnd != hwnd )
				SendMessage(m_hWnd, CMouseHook::UWM_DRAGEEND,(WPARAM)hwnd,0);
			//CMouseHook::StopHook();
		}
	}

    if(nCode < 0)
	{	/* pass it on */
		 CallNextHookEx(m_hHook, nCode, wParam, lParam);
		 return 0;
	} /* pass it on */

	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
} // msghook


BOOL CMouseHook::StartHook(HWND hWnd)
{
	if( !m_hHook )
	{
		m_hHook = SetWindowsHookEx(WH_GETMESSAGE,
				(HOOKPROC)GetMsgProc,
				m_hModule,
				0);

		if( m_hHook )
		{
			m_hWnd = hWnd;
			return TRUE;
		}
		else
		{
			ErrorMsg();
			return FALSE;
		}
	}
	return TRUE;
}

BOOL  CMouseHook::StopHook()
{
	if(m_hHook )
	{
		if(UnhookWindowsHookEx(m_hHook))
		{
			m_hHook = NULL;
			m_hWnd = NULL;
		} else
		{
			 ErrorMsg();
			 return FALSE;
		}		
	} 
	return TRUE;
}