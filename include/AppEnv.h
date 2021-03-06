#pragma once

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

struct AppEnv
{
	static TCHAR szAppPath[_MAX_PATH];
	static TCHAR szMediaPath[_MAX_PATH];
	static TCHAR szEngine[_MAX_PATH];
	static TCHAR szTheme[_MAX_PATH];
	static int	nThinkTime;
	static BOOL  bAutoPlay ;

	//@description
	//@param szSettingFile: MUST contain a directory message. 
	// ex: "./cchelp.ini" is ok, but "cchelper.ini" will be fail.
	static bool LoadEnv(TCHAR * szSettingFile);
	static TCHAR * GetMediaPath(const TCHAR * szMediaFile, const TCHAR * szTheme = NULL);
};

