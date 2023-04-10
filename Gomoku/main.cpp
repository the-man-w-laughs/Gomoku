#include <windows.h>
#include "gomoku.h"
#include <gdiplus.h>
#include "Negamax.h"
#include "Resource.h"
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")


Gomoku gomoku;
Negamax negamax(4);

const int kSquareSize{ 40 };

int mouse_board_x{ -1 };
int mouse_board_y{ -1 };

HINSTANCE g_hInst;

class CGdiPlusStarter {
private:
	ULONG_PTR m_gpToken;

public:
	bool m_bSuccess;
	CGdiPlusStarter() {
		GdiplusStartupInput gpsi;
		m_bSuccess = (GdiplusStartup(&m_gpToken, &gpsi, NULL) == Ok);
	}
	~CGdiPlusStarter() {
		GdiplusShutdown(m_gpToken);
	}
};

CGdiPlusStarter g_gps;

HWND hWndForTwoPlayers;
bool windowTwoPlayersRegisteredBefore = false;
void createWindowTwoPlayers(HWND& hWnd, HINSTANCE hInstance, int nShowCmd);
LRESULT CALLBACK WndProcTwoPlayers(HWND, UINT, WPARAM, LPARAM);

turn who;

HWND hWndPlayWithComputer;
bool windowPlayWithComputerRegisteredBefore = false;
void createWindowPlayWithComputer(HWND& hWnd, HINSTANCE hInstance, int nShowCmd);
LRESULT CALLBACK WndProcPlayWithComputer(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

HWND hWndMain;
bool windowMainRegisteredBefore = false;
void createWindowMain(HWND& hWnd, HINSTANCE hInstance, int nShowCmd);
LRESULT CALLBACK WndProcMain(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

windowtoopenenum windowToOpen;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow) {
	MSG Message;
	g_hInst = hInstance;

	if (g_gps.m_bSuccess == FALSE) {
		MessageBox(NULL, TEXT("GDI+ failed to start."),
			TEXT("Error"), MB_OK);
		return 0;
	}
	createWindowMain(hWndMain, hInstance, nCmdShow);
	windowToOpen = NoWindow;
	while (windowToOpen != CloseProgram) {
		if (GetMessage(&Message, NULL, 0, 0));
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		switch (windowToOpen) {
		case mainWindow:
			createWindowMain(hWndMain, hInstance, nCmdShow);
			windowToOpen = NoWindow;
			break;
		case GameWithComputerWindow:
			createWindowPlayWithComputer(hWndPlayWithComputer, hInstance, nCmdShow);
			windowToOpen = NoWindow;
			break;
		case TwoPlayersWindow:
			createWindowTwoPlayers(hWndForTwoPlayers, hInstance, nCmdShow);
			windowToOpen = NoWindow;
			break;
		}
		
	}
	return (int)Message.wParam;
}
int xShift;
int yShift;

VOID OnPaint(HDC hdc)
{
	RECT crt;
	GetClientRect(hWndMain, &crt);

	HDC hdcBuffer = CreateCompatibleDC(hdc);  // OFF screen DC

	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);  // create memory bitmap for that off screen DC

	SelectObject(hdcBuffer, hBitmapBuffer); // Select the created memory bitmap into the OFF screen DC
	xShift = (crt.right - kSquareSize * gomoku.board_length()) / 2;
	yShift = (crt.bottom - kSquareSize * gomoku.board_length()) / 2;
	Graphics graphics(hdcBuffer);

	//board orange rectange draw
	graphics.FillRectangle(new SolidBrush(Color(231, 187, 95)), 0, 0, crt.right, crt.bottom);

	Pen pen(Color(0, 0, 0));
	for (int i = 1; i <= gomoku.board_length(); ++i) {
		graphics.DrawLine(&pen, xShift + i * kSquareSize - kSquareSize / 2,
			yShift + kSquareSize - kSquareSize / 2,
			xShift + i * kSquareSize - kSquareSize / 2,
			yShift + kSquareSize * gomoku.board_length() + 1 - kSquareSize / 2);
	}
	for (int i = 1; i <= gomoku.board_length(); ++i) {
		graphics.DrawLine(&pen, xShift + kSquareSize - kSquareSize / 2,
			yShift + i * kSquareSize - kSquareSize / 2,
			xShift + kSquareSize * gomoku.board_length() + 1 - kSquareSize / 2,
			yShift + i * kSquareSize - kSquareSize / 2);
	}


	SolidBrush brush_black(Color(0, 0, 0));
	SolidBrush brush_white(Color(255, 255, 255));

	const auto& board = gomoku.board;
	for (size_t x = 0; x < gomoku.board_length(); x++) {
		for (size_t y = 0; y < gomoku.board_length(); y++) {
			switch (board[x][y]) {
			case black:
				graphics.FillEllipse(&brush_black,
					xShift + x * kSquareSize, yShift + y * kSquareSize,
					kSquareSize, kSquareSize);
				break;
			case white:
				graphics.FillEllipse(&brush_white,
					xShift + x * kSquareSize, yShift + y * kSquareSize,
					kSquareSize, kSquareSize);
				break;
			}
		}
	}	


	//mouse preview piece with alpha
	SolidBrush brush_black_alpha(Color(128, 0, 0, 0));
	SolidBrush brush_white_alpha(Color(128, 255, 255, 255));

	//preview is have to in board
	//and nothing ont preview location
	if (0 <= mouse_board_x && mouse_board_x < gomoku.board_length() &&
		0 <= mouse_board_y && mouse_board_y < gomoku.board_length()
		&& board[mouse_board_x][mouse_board_y] == none) {

		switch (gomoku.getturn()) {
		case black:
			graphics.FillEllipse(&brush_black_alpha,
				xShift + mouse_board_x * kSquareSize,
				yShift + mouse_board_y * kSquareSize,
				kSquareSize, kSquareSize);
			break;
		case white:
			graphics.FillEllipse(&brush_white_alpha,
				xShift + mouse_board_x * kSquareSize,
				yShift + mouse_board_y * kSquareSize,
				kSquareSize, kSquareSize);
			break;
		}
	}

	/* Then do your painting job using hdcBuffer over off screen DC */

	BitBlt(hdc, 0, 0, crt.right, crt.bottom, hdcBuffer, 0, 0, SRCCOPY); // copy the content of OFF screen DC to actual screen DC

	DeleteDC(hdcBuffer); // Release the OFF screen DC

	DeleteObject(hBitmapBuffer); // Free the memory for bitmap
}

void createWindowPlayWithComputer(HWND& hWnd, HINSTANCE hInstance, int nShowCmd) {
	LPCTSTR windowPlayWithComputerClassName = TEXT("ComputerGomoku");
	LPCTSTR title = TEXT("Game with computer.");
	WNDCLASS windowPlayWithComputerClass;
	if (windowPlayWithComputerRegisteredBefore == false) {
		windowPlayWithComputerClass.cbClsExtra = 0;
		windowPlayWithComputerClass.cbWndExtra = 0;
		windowPlayWithComputerClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowPlayWithComputerClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowPlayWithComputerClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
		windowPlayWithComputerClass.hInstance = hInstance;
		windowPlayWithComputerClass.lpfnWndProc = WndProcPlayWithComputer;
		windowPlayWithComputerClass.lpszClassName = windowPlayWithComputerClassName;
		windowPlayWithComputerClass.lpszMenuName = NULL;
		windowPlayWithComputerClass.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClass(&windowPlayWithComputerClass))
		{
			int nResult = GetLastError();
			MessageBox(NULL,
				L"Window class creation failed",
				L"Window Class Failed",
				MB_ICONERROR);
		}
		else
			windowPlayWithComputerRegisteredBefore = true;
	}
	hWnd = CreateWindow(windowPlayWithComputerClassName, title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		kSquareSize * (gomoku.board_length() + 1),
		kSquareSize * (gomoku.board_length() + 2),
		NULL, (HMENU)NULL, hInstance, NULL);

	if (!hInstance)
	{
		int nResult = GetLastError();

		MessageBox(NULL,
			L"Window creation failed",
			L"Window Creation Failed",
			MB_ICONERROR);
	}

	ShowWindow(hWnd, nShowCmd);
}


LRESULT CALLBACK WndProcPlayWithComputer(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	static HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
	HDC hdc;
	PAINTSTRUCT ps;
	static int mouse_x;
	static int mouse_y;

	switch (iMessage) {
	case WM_CREATE:
		who = human;
		hWndMain = hWnd;
		SetTimer(hWnd, 1, 25, NULL);

		if (who == computer) {
			Move move = negamax.getMove(black_to_play, gomoku.board);

			mouse_board_x = move.i;
			mouse_board_y = move.j;

			if (!gomoku.PieceOnBoard(mouse_board_x, mouse_board_y))
				//if fail PieceOnBoard
				return 0;
		}

		return 0;


	case WM_TIMER:
		hdc = GetDC(hWnd);
		OnPaint(hdc);
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_LBUTTONDOWN:
			if (0 <= mouse_board_x && mouse_board_x < gomoku.board_length()
				&& 0 <= mouse_board_y && mouse_board_y < gomoku.board_length()) {

				if (!gomoku.PieceOnBoard(mouse_board_x, mouse_board_y))
					//if fail PieceOnBoard
					return 0;				

				if (gomoku.CheckWin()) {
					switch (gomoku.getturn()) {
					case black:
						MessageBox(hWnd, L"Black won", L"Game over", MB_OK);
						break;
					case white:
						MessageBox(hWnd, L"White won", L"Game over", MB_OK);
						break;
					}
					//reset
					gomoku.reset();
				}
				else {
					gomoku.PassTurn();
					who = computer;
					Move move = negamax.getMove(black_to_play, gomoku.board);

					mouse_board_x = move.i;
					mouse_board_y = move.j;

					if (!gomoku.PieceOnBoard(mouse_board_x, mouse_board_y))
						//if fail PieceOnBoard
						return 0;

					if (gomoku.CheckWin()) {
						switch (gomoku.getturn()) {
						case black:
							MessageBox(hWnd, L"Black won", L"Game over", MB_OK);
							break;
						case white:
							MessageBox(hWnd, L"White won", L"Game over", MB_OK);
							break;
						}
						//reset
						gomoku.reset();
					}
					else {
						gomoku.PassTurn();
					}
				}
				hdc = GetDC(hWnd);
				OnPaint(hdc);
				ReleaseDC(hWnd, hdc);
			}
		return 0;

	case WM_MOUSEMOVE:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		mouse_board_x = (mouse_x - xShift) / kSquareSize;
		mouse_board_y = (mouse_y - yShift) / kSquareSize;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		windowToOpen = mainWindow;
		PostQuitMessage(0);
		return 0;
	}


	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void createWindowTwoPlayers(HWND& hWnd, HINSTANCE hInstance, int nShowCmd) {
	LPCTSTR windowForTwoPlayersClassName = TEXT("TwoPlayerGomoku");
	LPCTSTR title = TEXT("Two player game.");
	WNDCLASS windowForTwoPlayersClass;
	if (windowTwoPlayersRegisteredBefore == false) {
		windowForTwoPlayersClass.cbClsExtra = 0;
		windowForTwoPlayersClass.cbWndExtra = 0;
		windowForTwoPlayersClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowForTwoPlayersClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowForTwoPlayersClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
		windowForTwoPlayersClass.hInstance = hInstance;
		windowForTwoPlayersClass.lpfnWndProc = WndProcTwoPlayers;
		windowForTwoPlayersClass.lpszClassName = windowForTwoPlayersClassName;
		windowForTwoPlayersClass.lpszMenuName = NULL;
		windowForTwoPlayersClass.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClass(&windowForTwoPlayersClass))
		{
			int nResult = GetLastError();
			MessageBox(NULL,
				L"Window class creation failed",
				L"Window Class Failed",
				MB_ICONERROR);
		}
		else
			windowTwoPlayersRegisteredBefore = true;
	}
	hWnd = CreateWindow(windowForTwoPlayersClassName, title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		kSquareSize * (gomoku.board_length() + 1),
		kSquareSize * (gomoku.board_length() + 2),
		NULL, (HMENU)NULL, hInstance, NULL);

	if (!hInstance)
	{
		int nResult = GetLastError();

		MessageBox(NULL,
			L"Window creation failed",
			L"Window Creation Failed",
			MB_ICONERROR);
	}

	ShowWindow(hWnd, nShowCmd);
}


LRESULT CALLBACK WndProcTwoPlayers(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	static HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
	HDC hdc;
	PAINTSTRUCT ps;
	static int mouse_x;
	static int mouse_y;

	switch (iMessage) {
	case WM_CREATE:
		who = human;
		hWndMain = hWnd;
		SetTimer(hWnd, 1, 25, NULL);
		return 0;

	case WM_TIMER:
		hdc = GetDC(hWnd);
		OnPaint(hdc);
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_LBUTTONDOWN:
		if (0 <= mouse_board_x && mouse_board_x < gomoku.board_length()
			&& 0 <= mouse_board_y && mouse_board_y < gomoku.board_length()) {

			if (!gomoku.PieceOnBoard(mouse_board_x, mouse_board_y))
				//if fail PieceOnBoard
				return 0;			
			if (gomoku.CheckWin()) {
				switch (gomoku.getturn()) {
				case black:
					MessageBox(hWnd, L"Black won", L"Game over", MB_OK);
					break;
				case white:
					MessageBox(hWnd, L"White won", L"Game over", MB_OK);
					break;
				}
				//reset
				gomoku.reset();
			}		
			else {
				gomoku.PassTurn();
			}
			hdc = GetDC(hWnd);
			OnPaint(hdc);
			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_MOUSEMOVE:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);

		mouse_board_x = (mouse_x - xShift) / kSquareSize;
		mouse_board_y = (mouse_y - yShift) / kSquareSize;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		windowToOpen = mainWindow;
		PostQuitMessage(0);
		return 0;
	}


	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
HWND hBtnFriend, hBtnComputer;
int MainWindowHeigth = 400;
int MainWindowWidth = 600;

int buttonWidth = 200;
int buttonHeight = 50;

VOID OnPaintMain(HDC hdc)
{
	RECT crt;
	GetClientRect(hWndMain, &crt);

	HDC hdcBuffer = CreateCompatibleDC(hdc);  // OFF screen DC

	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);  // create memory bitmap for that off screen DC

	SelectObject(hdcBuffer, hBitmapBuffer); // Select the created memory bitmap into the OFF screen DC

	Graphics graphics(hdcBuffer);
	
	//board orange rectange draw
	graphics.FillRectangle(new SolidBrush(Color(231, 187, 95)), 0, 0, crt.right, crt.bottom);

	/* Then do your painting job using hdcBuffer over off screen DC */

	BitBlt(hdc, 0, 0, crt.right, crt.bottom, hdcBuffer, 0, 0, SRCCOPY); // copy the content of OFF screen DC to actual screen DC

	DeleteDC(hdcBuffer); // Release the OFF screen DC

	DeleteObject(hBitmapBuffer); // Free the memory for bitmap
}

void createWindowMain(HWND& hWnd, HINSTANCE hInstance, int nShowCmd) {
	LPCTSTR windowMainClassName = TEXT("MainWindow");
	LPCTSTR title = TEXT("Gomoku");
	WNDCLASS windowMainClass;
	if (windowMainRegisteredBefore == false) {
		windowMainClass.cbClsExtra = 0;
		windowMainClass.cbWndExtra = 0;
		windowMainClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowMainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowMainClass.hIcon = LoadIcon(hInstance,MAKEINTRESOURCEW(IDI_ICON1));
		windowMainClass.hInstance = hInstance;
		windowMainClass.lpfnWndProc = WndProcMain;
		windowMainClass.lpszClassName = windowMainClassName;
		windowMainClass.lpszMenuName = NULL;
		windowMainClass.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClass(&windowMainClass))
		{
			int nResult = GetLastError();
			MessageBox(NULL,
				L"Window class creation failed",
				L"Window Class Failed",
				MB_ICONERROR);
		}
		else
			windowMainRegisteredBefore = true;
	}
	hWnd = CreateWindow(windowMainClassName, title, WS_OVERLAPPEDWINDOW ,
		CW_USEDEFAULT, CW_USEDEFAULT,
		MainWindowWidth,
		MainWindowHeigth,
		NULL, (HMENU)NULL, hInstance, NULL);

	if (!hInstance)
	{
		int nResult = GetLastError();

		MessageBox(NULL,
			L"Window creation failed",
			L"Window Creation Failed",
			MB_ICONERROR);
	}

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);
}

LRESULT CALLBACK WndProcMain(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	Graphics G(hWndMain);
	Graphics* memG;
	Bitmap* pBit;
	HDC hdc;
	static HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;
	RECT cr;
	int x, y;
	PAINTSTRUCT ps;
	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &cr);

		pBit = new Bitmap(cr.right, cr.bottom, &G);
		memG = new Graphics(pBit);

		//board orange rectange draw
		memG->FillRectangle(new SolidBrush(Color(231, 187, 95)), 0, 0, (gomoku.board_length() + 1) * kSquareSize, (gomoku.board_length() + 1) * kSquareSize);
		InvalidateRect(hWndMain, NULL, FALSE);
		
		x = (int)(cr.right - buttonWidth) / 2;
		y = cr.bottom * 1 / 4;

		hBtnFriend = CreateWindow(L"button", L"Play with a FRIEND", WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, buttonWidth, buttonHeight, hWnd, 0, hInst, NULL);
		ShowWindow(hBtnFriend, SW_SHOWNORMAL);

		y = cr.bottom * 2 / 4;
		hBtnComputer = CreateWindow(L"button", L"Play with a COMPUTER", WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, buttonWidth, buttonHeight, hWnd, 0, hInst, NULL);
		ShowWindow(hBtnComputer, SW_SHOWNORMAL);
		return 0;
	case WM_SIZE:
		GetClientRect(hWndMain, &cr);
		SetWindowPos(hBtnFriend, HWND_TOP, (cr.right - buttonWidth) / 2, cr.bottom * 1 / 4, NULL, NULL, SWP_NOSIZE);
		SetWindowPos(hBtnComputer, HWND_TOP, (cr.right - buttonWidth) / 2, cr.bottom * 1 / 4 + 2 * buttonHeight, NULL, NULL, SWP_NOSIZE);
		break;
	case WM_COMMAND:
		if (lParam == (LPARAM)hBtnFriend) {
			windowToOpen = TwoPlayersWindow;
		}
		if (lParam == (LPARAM)hBtnComputer) {
			windowToOpen = GameWithComputerWindow;
		}
		gomoku.reset();
		DestroyWindow(hWnd);
		break;		
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaintMain(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		if (windowToOpen == NoWindow)
		windowToOpen = CloseProgram;
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}