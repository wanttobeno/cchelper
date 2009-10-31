#include "common.h"
#include "QQNewChessWnd.h"

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
}

CQQNewChessWnd::~CQQNewChessWnd(void)
{
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

int CQQNewChessWnd::GetTurn()
{
	int ret = TURN_NONE ;

	DWORD key1 = 0;
	DWORD key2 = 0;

	key1 = GetHashValue( TURN1_X, TURN1_Y, SAMPLE_LEN);
	key2 = GetHashValue( TURN2_X, TURN2_Y, SAMPLE_LEN);

	//base::Log(1, "key1=%p, key2=%p", key1,key2);
	if( key1 == TURN_WHITE_KEY || key2 == TURN_WHITE_KEY ) {
		ret = TURN_WHITE ;
	}
	else if( key1 == TURN_BLACK_KEY || key2 == TURN_BLACK_KEY)	{
		ret = TURN_BLACK ;
	}


	return ret;
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

COLORREF * CQQNewChessWnd::GetRectData(int x, int y)
 {
	 COLORREF * color;
	 color = new COLORREF[58*58];
	 int count = 0;

	 if ( ! this->GetHandle() )
	 {
		 return NULL;
	 }

	 HDC hdc = GetDC(this->GetHandle() );

	 for( int i = 0 ; i < 58; i ++) 
		 for( int j = 0; j < 58 ; j ++ )
		 {
			 color[j*58+i] = GetPixel( hdc, ox + x * dx + i , oy + y * dy + j);
		 }

	 ReleaseDC(this->m_hwndMain, hdc);

	 return color;
 }

bool CQQNewChessWnd::Attach(HWND hwnd)
{
	int i;

	//OutputDebugString(_T("=============GetItems = Start\n"));

	RealGetWindowClass(hwnd, m_szWindowClass, sizeof(m_szWindowClass));
	//OutputDebugString(_T("WindowClass="));
	//OutputDebugString(m_szWindowClass);
	//OutputDebugString(_T("\n"));

	if ( wcsncmp( qq_classid, m_szWindowClass, wcslen(qq_classid) ) != 0)
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

	//OutputDebugString(_T("=============GetItems=== End\n"));
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
