#include "common.h"
#include "QQNewChessWnd.h"

#define BUTTON_ID_CANCEL	(2)
#define BUTTON_ID_OK		(1)

#define HASH_SEED		(1689)
#define SAMPLE_LEN	(20)
#define SAMPLE_SX	(15)
#define SAMPLE_SY	(15)

#define TURN1_X	(155+15)
#define TURN1_Y (270+15)

#define TURN2_X (155+15)
#define TURN2_Y (452+15)

#define TURN_WHITE_KEY	0x9c07432e
#define TURN_BLACK_KEY	0x5818880b

#define PIECE_NUM	(14)

#define PIECEHV_RJ	(0xD2FC85BF)
#define PIECEHV_RM	(0x17B56D39)
#define PIECEHV_RX	(0xC4F9C567)
#define PIECEHV_RS	(0x826AA68B)
#define PIECEHV_RK	(0x18233962)
#define PIECEHV_RP	(0x402355D8)
#define PIECEHV_RB	(0x03A331A5)

#define PIECEHV_BJ	(0x56FC44E8)
#define PIECEHV_BM	(0x5699E66C)
#define PIECEHV_BX	(0x7AA76288)
#define PIECEHV_BS	(0x2BC9BFCC)
#define PIECEHV_BK	(0x651E8CBB)
#define PIECEHV_BP	(0x737E5B02)
#define PIECEHV_BB	(0x9291F591)


const TCHAR  * qq_classid = _T("Afx:400000:b:10003:6");

struct PieceHashValue
{
	char cPiece;
	DWORD dwPieceHashValue;
};

#ifdef ENGINE_CCE

PieceHashValue _PieceHashValues[PIECE_NUM] = {
	{ 'J', PIECEHV_RJ },
	{ 'M', PIECEHV_RM },
	{ 'X', PIECEHV_RX },
	{ 'S', PIECEHV_RS },
	{ 'K', PIECEHV_RK },
	{ 'P', PIECEHV_RP },
	{ 'B', PIECEHV_RB },
	{ 'j', PIECEHV_BJ },
	{ 'm', PIECEHV_BM },
	{ 'x', PIECEHV_BX },
	{ 's', PIECEHV_BS },
	{ 'k', PIECEHV_BK },
	{ 'p', PIECEHV_BP },
	{ 'b', PIECEHV_BB },
};

#else
PieceHashValue _PieceHashValues[PIECE_NUM] = {
	{ 'R', PIECEHV_RJ },
	{ 'N', PIECEHV_RM },
	{ 'B', PIECEHV_RX },
	{ 'A', PIECEHV_RS },
	{ 'K', PIECEHV_RK },
	{ 'C', PIECEHV_RP },
	{ 'P', PIECEHV_RB },
	{ 'r', PIECEHV_BJ },
	{ 'n', PIECEHV_BM },
	{ 'b', PIECEHV_BX },
	{ 'a', PIECEHV_BS },
	{ 'k', PIECEHV_BK },
	{ 'c', PIECEHV_BP },
	{ 'p', PIECEHV_BB },
};

#endif
QncDlgItemInfo CQQNewChessWnd::QncItemsInfo[QNC_ITEMSIZE] = 
{
		{  0x960, _T("Button"), { 55, 31 } }, 
		{  0x961, _T("Button"), { 55, 31 } },
		{  0x962, _T("Button"), { 55, 31 } },
		{  0x963, _T("Button"), { 55, 31 } }, 
		{  0x964, _T("Button"), { 55, 31 } }, 
		{  0x965, _T("Button"), { 55, 31 } }, 
};



CQQNewChessWnd::CQQNewChessWnd(void)
{
	this->m_hwndMain = NULL;
	memset(this->m_hwndItems , 0, sizeof(this->m_hwndItems));
	ox = 242;
	oy = 43;
	dx = 57; 
	dy = 57;
	
	m_bGameOver = true;
	m_pChessEngine = NULL;
	m_hwndFrame = NULL;

}

CQQNewChessWnd::~CQQNewChessWnd(void)
{
}


bool CQQNewChessWnd::ReadGameWindowInfo()
{

	if( !GetHandle() )
	{
		return false;
	}

	GAMEWINDOWINFO gwi;

	memset(&gwi, 0, sizeof(gwi));

	bool ret = false;

	BOOL bStartEnable, bDrawnEnable, bLostEnable, bRetractEnable, bToolsEnable, bDiscsEnable;

	bStartEnable	= IsWindowEnabled(m_hwndItems[QNC_START_BTN]);
	bDrawnEnable	= IsWindowEnabled(m_hwndItems[QNC_DRAWN_BTN]);
	bLostEnable		= IsWindowEnabled(m_hwndItems[QNC_LOST_BTN]);
	bRetractEnable	= IsWindowEnabled(m_hwndItems[QNC_RETRACT]);
	bToolsEnable	= IsWindowEnabled(m_hwndItems[QNC_TOOLS_BTN]);
	bDiscsEnable	= IsWindowEnabled(m_hwndItems[QNC_DISCS_BTN]);


	if(!bStartEnable && !bDrawnEnable && !bLostEnable && !bRetractEnable && !bToolsEnable && bDiscsEnable)
	{
		gwi.bObserverMode = true;
	} 
	else
	{
		gwi.bObserverMode = false;
	}
	
	if( ReadFen(&gwi) )
	{
 		if(bStartEnable || 
			(!bStartEnable && !bDrawnEnable && !bLostEnable && !bRetractEnable && !bToolsEnable && !bDiscsEnable ) )
		{
			if( !m_bGameOver )
			{
				m_bGameOver = true;
				if( m_pChessEngine )
				{
					m_pChessEngine->GameOver();
				}
			}
		}
		else
		{
			if ( m_bGameOver )
			{
				m_bGameOver = false;
				if( m_pChessEngine )
				{
					m_pChessEngine->Restart();
				}
			}
		}

		if( m_GameWindowInfo.bAvailible )
		{
			if(strcmp(m_GameWindowInfo.szFen, gwi.szFen ) != 0 )
			{
				if( m_GameWindowInfo.Turn != gwi.Turn )
				{
					gwi.bTurnChanged = true;
					gwi.bAvailible = true;
				} 
				else
				{
					gwi.bAvailible = false;
				}
			}
			else
			{
				if( m_GameWindowInfo.Turn != gwi.Turn )
				{
					gwi.bAvailible = false;					
				}else
				{
					gwi.bAvailible = true;
				}
			}
		} 
		else 
		{
			gwi.bAvailible = true;
		}
	} 
	else
	{
		gwi.bAvailible = false;
	}

	m_GameWindowInfo = gwi;

	return m_GameWindowInfo.bAvailible ;
}

bool CQQNewChessWnd::ReadTurn(GAMEWINDOWINFO * pgi)
{
	pgi->Turn = TURN_NONE ;
	pgi->PlayerColor = TURN_NONE;

	DWORD key1 = 0;
	DWORD key2 = 0;

	key1 = GetHashValue( TURN1_X, TURN1_Y, SAMPLE_LEN);
	key2 = GetHashValue( TURN2_X, TURN2_Y, SAMPLE_LEN);

	if( key1 == TURN_WHITE_KEY )
	{
		pgi->Turn  = TURN_WHITE ;
		pgi->PlayerColor = TURN_BLACK;
	} else if (key2 == TURN_WHITE_KEY ) {
		pgi->Turn  = TURN_WHITE ;
		pgi->PlayerColor = TURN_WHITE;		
	} else if( key1 == TURN_BLACK_KEY ) {
		pgi->Turn  = TURN_BLACK;
		pgi->PlayerColor = TURN_WHITE;
	} else if ( key2 == TURN_BLACK_KEY)	{
		pgi->Turn  = TURN_BLACK ;
		pgi->PlayerColor = TURN_BLACK;
	}

	if ( pgi->Turn == TURN_NONE )
		return false;

	return true;
}


bool CQQNewChessWnd::ReadFen(GAMEWINDOWINFO * pgi)
{
	assert(pgi);

	if ( !ReadTurn(pgi) )
	{
		pgi->szFen[0] = 0;
		return false;
	}
	
	char * pFen = pgi->szFen ;

	int p;
	int x;
	int y;

	int count = 0;

	for( y =0; y < 10; y++ )
	{
		for( x = 0; x < 9; x ++)
		{
			if( pgi->PlayerColor == TURN_WHITE) p = GetPiece(x,y);
			else if(pgi->PlayerColor == TURN_BLACK) p = GetPiece(x, 9 - y );
			if ( p )
			{
				if ( count > 0 )
				{
					*pFen++ = '0' + count;
					count = 0;
				}
				*pFen++ = p;
			}
			else
			{
				count ++;
			}
		}
		if ( count > 0 )
		{
			*pFen++ = '0' + count;
			count = 0;
		}
		if( y < 9 )
			*pFen++ = '/';
	}
	*pFen++ = ' ';
	*pFen++ = pgi->Turn ;
	*pFen++ = 0;

	return true;
}

BOOL CALLBACK QQNewChessWnd_EnumChildProc(  HWND hwnd,  LPARAM lParam)
{
	CQQNewChessWnd * pQncWnd = (CQQNewChessWnd *) lParam;

	WINDOWINFO wi;

	TCHAR buf[256];

	RealGetWindowClass(hwnd, buf, sizeof(buf));
	//OutputDebugString(_T("ChildWindowClass="));
	//OutputDebugString(buf);
	_stprintf_s( buf , _T(" CtrlID=0x%p"),  GetDlgCtrlID( hwnd ));
	//OutputDebugString(buf);
	//OutputDebugString(_T("\n"));

	int i;

	for( i = 0 ; i < QNC_ITEMSIZE; i++)
	{
		if ( GetDlgCtrlID( hwnd ) == CQQNewChessWnd::QncItemsInfo[i].nID )
		{
			memset(&wi, 0, sizeof(wi));

			GetWindowInfo(hwnd, &wi);
			_stprintf_s(buf, _T("Size(%d,%d)\n"), (wi.rcWindow.right - wi.rcWindow.left), (wi.rcWindow.bottom - wi.rcWindow.top));

			//OutputDebugString(buf);
			if ( (wi.rcWindow.right - wi.rcWindow.left)  == CQQNewChessWnd::QncItemsInfo[i].ptSize.x 
				&& (wi.rcWindow.bottom - wi.rcWindow.top)  == CQQNewChessWnd::QncItemsInfo[i].ptSize.y )
			{
				pQncWnd->m_hwndItems[i] = hwnd;
			} 
		}
	}

	return true;
}

BOOL CQQNewChessWnd::MovePiece(IChessEngine::PieceMove * pmv )
{
	assert(pmv);

	HWND hWnd = this->m_hwndMain ;
	
	if( !hWnd )	return FALSE;

	int fx, fy, tx, ty;

	fx = pmv->from.x ;
	fy = pmv->from.y ;
	tx = pmv->to.x ;
	ty = pmv->to.y ;

	if( this->GetGameWindowInfo().PlayerColor == TURN_BLACK) 
	{
		fy = 9 - fy;
		ty = 9 - ty;
	}

	RECT rt;

	GetWindowRect(hWnd, &rt);

	int sx = dx / 2;
	int sy = dy / 2;

	POINT ptSave;

	POINT ptSrc, ptDst;

	GetCursorPos(&ptSave);

	ptSrc.x = rt.left + ox + fx * dx + sx;
	ptSrc.y = rt.top + oy + fy*dy + sy;

	ptDst.x = rt.left + ox + tx * dx + sx;
	ptDst.y = rt.top + oy + ty*dy + sy;

	HWND hwnd1 = WindowFromPoint(ptSrc);
	HWND hwnd2 = WindowFromPoint(ptDst);
	if( hwnd2 != this->m_hwndMain || hwnd1 != this->m_hwndMain   )
	{
		return FALSE;
	}


	INPUT in[2];

	memset(in, 0, sizeof(INPUT) * 2);

	// click the piece position.
	in[0].type = INPUT_MOUSE;
	in[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	in[0].mi.dx = ptSrc.x ;
	in[0].mi.dy = ptSrc.y;

	in[1].type = INPUT_MOUSE;
	in[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	in[1].mi.dx = ptSrc.x ;
	in[1].mi.dy = ptSrc.y;

	SetCursorPos( ptSrc.x , ptSrc.y );	
	SendInput(2, in, sizeof(INPUT));


	// click the taget position.
	in[0].type = INPUT_MOUSE;
	in[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	in[0].mi.dx = ptDst.x;
	in[0].mi.dy = ptDst.y;

	in[1].type = INPUT_MOUSE;
	in[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	in[1].mi.dx = ptDst.x ;
	in[1].mi.dy = ptDst.y ;

	SetCursorPos( ptDst.x, ptDst.y );
	SendInput(2, in, sizeof(INPUT));

	SetCursorPos( ptSave.x, ptSave.y );

	return TRUE;

}


DWORD CQQNewChessWnd::GetHashValue( int x, int y, int len )
{
	if( !this->GetHandle() )
		return 0;
	
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	DWORD h = HASH_SEED ^ (len * sizeof(DWORD));

	DWORD k;

	HDC hdc = GetDC(this->GetHandle() );

	while( len-- )
	{
		k = GetPixel( hdc, x ++ , y ++);

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;
	}

	ReleaseDC(this->m_hwndMain, hdc);


	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}


DWORD CQQNewChessWnd::GetHashValue( int x, int y )
{
	int sx = SAMPLE_SX;
	int sy = SAMPLE_SY;
	int len = SAMPLE_LEN;

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	DWORD h = HASH_SEED ^ (len * sizeof(DWORD));

	DWORD k;

	HDC hdc = GetDC(this->GetHandle() );

	if ( hdc )
	{
		while( len )
		{
			k = GetPixel( hdc, ox + x * dx + sx , oy + y * dy + sy);

			k *= m; 
			k ^= k >> r; 
			k *= m; 
			
			h *= m; 
			h ^= k;

			sx ++;
			sy ++;
			len --;
		}

		ReleaseDC(this->GetHandle(), hdc);

		// Do a few final mixes of the hash to ensure the last few
		// bytes are well-incorporated.

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;
	}
	return h;
}

char CQQNewChessWnd::GetPiece( int x, int y)
{
	char ret = 0;
	DWORD k;
	k = GetHashValue(x,y);

	for( int i=0; i < PIECE_NUM; i++) {
		if ( _PieceHashValues[i].dwPieceHashValue == k )
		{
			ret = _PieceHashValues[i].cPiece ;
			break;
		}
	}

	return ret;
}


void CQQNewChessWnd::DumpBoradHash()
{
	char buf[1024];

	DWORD k;
	int i;
	int j;

	for( j =0; j < 10; j++ )
	{
		for( i = 0; i < 9; i ++)
		{
			k = GetHashValue(i,j);
			sprintf_s(buf, "[%4p]", k);
			base::Log(1, buf );
		}
	}
}

bool CQQNewChessWnd::Attach(HWND hwnd)
{
	int i;

	//OutputDebugString(_T("=============GetItems = Start\n"));

	RealGetWindowClass(hwnd, m_szWindowClass, sizeof(m_szWindowClass));
	//OutputDebugString(_T("WindowClass="));
	//OutputDebugString(m_szWindowClass);
	//OutputDebugString(_T("\n"));

	if ( _tcsncmp( qq_classid, m_szWindowClass, _tcslen(qq_classid) ) != 0)
	{
		return false;
	}

	EnumChildWindows(hwnd, QQNewChessWnd_EnumChildProc, (LPARAM) this);

	for( i = 0 ; i < QNC_ITEMSIZE; i++)
	{
		if ( this->m_hwndItems[i] == NULL )
		{
			return false;
		}
	}

	this->m_hwndMain = GetParent(m_hwndItems[0]);
	m_hwndFrame = hwnd;
	//OutputDebugString(_T("=============GetItems=== End\n"));

	memset(&m_GameWindowInfo, 0, sizeof(m_GameWindowInfo));
	return true;
}

BOOL CALLBACK MyEnumWindowsProc( HWND hwnd,    LPARAM lParam)
{
	CQQNewChessWnd * pQncWnd = (CQQNewChessWnd*) lParam;

	if( pQncWnd->Attach(hwnd) )
	{
		return FALSE;
	}
	
	return TRUE;
}

HWND CQQNewChessWnd::FindQQNewChessWindow()
{
	EnumWindows( MyEnumWindowsProc,(LPARAM)this);

	return (this->GetHandle () );	
}
