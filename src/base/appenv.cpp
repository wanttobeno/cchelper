#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "log.h"
#include "appenv.h"

using namespace base;

TCHAR AppEnv::szAppPath[_MAX_PATH];
TCHAR AppEnv::szMediaPath[_MAX_PATH];

bool AppEnv::LoadEnv(TCHAR * szSettingFile)
{

	_tgetcwd(AppEnv::szAppPath, sizeof(AppEnv::szAppPath));
	if(!GetPrivateProfileString(
		_T("AppEnv"), 
		_T("MediaPath"), 
		_T("../../media/"), 
		AppEnv::szMediaPath, 
		sizeof(AppEnv::szMediaPath), 
		szSettingFile
		))
	{
		base::Log(1, "read ini failed %x", GetLastError());
	}

	return true;
}

TCHAR * AppEnv::GetMediaPath(TCHAR * szMediaFile)
{
	static TCHAR szBuf[_MAX_PATH];

	_sntprintf(szBuf, sizeof(szBuf), _T("%s%s"), AppEnv::szMediaPath, szMediaFile);

	return szBuf;
}

