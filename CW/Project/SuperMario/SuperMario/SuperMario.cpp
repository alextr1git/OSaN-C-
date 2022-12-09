/*// SuperMario.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "SuperMario.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SUPERMARIO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SUPERMARIO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUPERMARIO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SUPERMARIO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        LoadResources();
        InitializeBall(hWnd);
        SetTimer(hWnd, timer, 1, NULL);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case WM_TIMER:
                RecalculateBallSpeed();
                RecalculateBallPosition();
                Gravity();
                InvalidateRect(hWnd, NULL, TRUE);//обновить область рисования
                break;
            case WM_KEYDOWN:
                switch (wParam) {
                case VK_LEFT:
                    SetUpLeftHit();
                    break;
                case VK_RIGHT:
                    SetUpRightHit();
                    break;
                case VK_UP:
                    SetUpUpHit();
                    break;
                case VK_DOWN:
                    SetUpDownHit();
                    break;
                }
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            BOOL result = DrawBitmap(hdc, ball.X - ball.Radius, ball.Y - ball.Radius, hBmpBall);
            if (result == FALSE) {
                MessageBox(NULL,
                    _T("Call to DrawBitmap failed!"),
                    _T("Windows Desktop Guided Tour"),
                    NULL);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


BOOL DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
    HBITMAP hBmp, hBmpOld;
    HDC hMemDC;
    BITMAP bmp;
    POINT ptSize, ptOrg;

    hMemDC = CreateCompatibleDC(hDC); // creating memory context, that is compatible with hDC
    //Чтобы приложения могли размещать выходные данные в памяти, а не отправлять их на фактическое устройство
    if (hMemDC == NULL) {
        return FALSE;
    }


    hBmpOld = (HBITMAP)SelectObject(hMemDC, hBitmap); // select image into the context	
    // this function returns ID of the 
    //BMP that was loaded into the memory context earlier
    if (!hBmpOld) {
        return FALSE;
    }

    SetMapMode(hMemDC, GetMapMode(hDC)); // synchronizing of the memory context and showing context
    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);

    ptSize.x = bmp.bmWidth;
    ptSize.y = bmp.bmHeight;
    DPtoLP(hDC, &ptSize, 1); // convert units into logical units

    ptOrg.x = 0;
    ptOrg.y = 0;
    DPtoLP(hDC, &ptSize, 1);

   // BitBlt(hDC, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, BLACKNESS);
    TransparentBlt(hDC, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, ptSize.x, ptSize.y, RGB(255, 255, 255));

    SelectObject(hMemDC, hBmpOld); // restore memory context
    DeleteObject(hMemDC);
}

void LoadResources()
{
    //hBmpBall = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
    hBmpBall = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    int Error = GetLastError();

}

void InitializeBall(HWND hWnd)
{
    BITMAP bmp;
    GetObject(hBmpBall, sizeof(BITMAP), (LPSTR)&bmp);
    ball.X = WND_WIDTH / 2;
    ball.Y = WND_HEIGHT / 2;
    ball.Radius = bmp.bmWidth / 2;
    ball.SpeedX = 0.0;
    ball.SpeedY = 0.0;
    ball.BoostX = 0.0;
    ball.BoostY = 0.0;
    ball.directionX = NONE_X;
    ball.directionY = NONE_Y;
}

void Gravity() {
    ball.Y = ball.Y + 1;
}

void RecalculateBallPosition()
{
    switch (ball.directionX) {
    case LEFT:
        ball.X -= ball.SpeedX - ball.BoostX / 2;
        if (LeftHitten()) { ball.directionX = RIGHT; }
        break;
    case RIGHT:
        ball.X += ball.SpeedX - ball.BoostX / 2;
        if (RightHitten()) { ball.directionX = LEFT; }
        break;
    case NONE_X:
        break;
    }

    switch (ball.directionY) {
    case UP:
        ball.Y -= ball.SpeedY - ball.BoostY / 2;
        if (TopHitten()) { ball.directionY = DOWN; }
        break;
    case DOWN:
        ball.Y += ball.SpeedY - ball.BoostY / 2;
        if (BottomHitten()) { ball.directionY = UP; }
        break;
    case NONE_Y:
        break;
    }
}
void RecalculateBallSpeed()
{
    if (ball.directionX != NONE_X) {
        ball.SpeedX = ball.SpeedX - ball.BoostX;
        if (ball.SpeedX > (-1) * ALLOWED_FAULT && ball.SpeedX < ALLOWED_FAULT) {
            ball.SpeedX = 0.0;
            ball.BoostX = 0.0;
            ball.directionX = NONE_X;
        }
    }

    if (ball.directionY != NONE_Y) {
        ball.SpeedY = ball.SpeedY - ball.BoostY;
        if (ball.SpeedY > (-1) * ALLOWED_FAULT && ball.SpeedY < ALLOWED_FAULT) {
            ball.SpeedY = 0.0;
            ball.BoostY = 0.0;
            ball.directionY = NONE_Y;
        }
    }
}
void SetUpLeftHit()
{
    ball.directionX = LEFT;
    ball.SpeedX = START_SPEED;
    ball.BoostX = BOOST;
}

void SetUpRightHit()
{
    ball.directionX = RIGHT;
    ball.SpeedX = START_SPEED;
    ball.BoostX = BOOST;
}

void SetUpUpHit()
{
    ball.directionY = UP;
    ball.SpeedY = START_SPEED;
    ball.BoostY = BOOST;
}

void SetUpDownHit()
{
    ball.directionY = DOWN;
    ball.SpeedY = START_SPEED;
    ball.BoostY = BOOST;
}

BOOL LeftHitten()
{
    if (ball.X - ball.Radius <= 0) { return TRUE; }
    else { return FALSE; }
}

BOOL RightHitten()
{
    if (ball.X + ball.Radius >= WND_WIDTH) { return TRUE; }
    else { return FALSE; }
}

BOOL TopHitten()
{
    if (ball.Y - ball.Radius <= 0) { return TRUE; }
    else { return FALSE; }
}

BOOL BottomHitten()
{
    if (ball.Y + ball.Radius >= WND_HEIGHT - 20) { return TRUE; }
    else { return FALSE; }
}*/

#include "SuperMario.h"
#include "resource.h"

bool mflag = false;
int X, Y = 0;
POINT pos; RECT rc;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,// Начальный адрес программы(дескриптор)
	_In_ HINSTANCE hPrevInstance, //null
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow //Как показать главное окно при запуске
)
{
	WNDCLASSEX wcex; //оконный класс

	wcex.cbSize = sizeof(WNDCLASSEX); //Выделение памяти под структуру необходимо делать
	//для последующей передачи по сети, 
   //чтобы знали размер требуемого буфера
	wcex.style = CS_HREDRAW | CS_VREDRAW;//	Внутренняя область окна должна быть 
	//перерисована при изменении ширины и высоты окна.									  
	wcex.lpfnWndProc = WndProc; //Процедура для обработки сообщений
	wcex.cbClsExtra = 0; // доп.память для оконного класса и окна
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(167, 227, 253));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass; // идентификатор оконного класса
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("The Ball"),
			NULL);

		return 1;
	}

	hInst = hInstance;

	HWND hWnd = CreateWindow( // создание окна в оп, его не видно. Возвращает дескриптор окна
		szWindowClass,
		szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(int)WND_WIDTH,
		(int)WND_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow); //Команда - показать, как можно быстрее, но когда - неизвестно.
	UpdateWindow(hWnd); //показать при первой же прорисовке

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); /*
		Только для клавиатурных сообщений.
		При вводе клавиши оно ставит в очередь еще одно сообщение и записывает туда
		из virtual code сам символ клавиатуры.
		При комментировании этой строки будет не работать обработчик WM_CHAR */
		DispatchMessage(&msg);/*
		Метод обработчик сообщений.
		Он позволяет не думать о лишнем при множестве классов
		и не нужно заниматься диспетчеризацией,
		как если бы мы вызывали сразу обработчик.
		В минимальной программе это можно, но не в реальной*/
	}

	return (int)msg.wParam; // Реальный скан-код клавиши траслируется в виртуальный код виндовс.
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message) {
	case WM_CREATE:
		LoadResources();
		InitializeBall(hWnd);
		SetTimer(hWnd, timer, 1, NULL);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0); //Помещает в очередь сообщений сообщение WM_quit
		//и тогда GetMessage вернет FALSE
		break;
	case WM_PAINT:
		DrawBall(hWnd);
		break;
	case WM_TIMER:
		RecalculateBallSpeed();
		RecalculateBallPosition();
		//Gravity();
		InvalidateRect(hWnd, NULL, TRUE);//обновить область рисования
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			SetUpLeftHit();
			break;
		case VK_RIGHT:
			SetUpRightHit();
			break;
		case VK_SPACE:
			SetUpUpHit();
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);/*
		Отвечает за работу инструментов окна (закрыть, скрыть, прокрутить...).
		Всегда вызывать как дефолт*/
		break;
	}

	return 0;
}

void LoadResources()
{
	//hBmpBall = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	hBmpBall = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	int Error = GetLastError();

}

void Gravity() {
	ball.Y = ball.Y + GRAVITATION;
}
void InitializeBall(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hBmpBall, sizeof(BITMAP), (LPSTR)&bmp);
	ball.X = WND_WIDTH / 2;
	ball.Y = WND_HEIGHT / 2;
	ball.Radius = bmp.bmWidth / 2;
	ball.SpeedX = 0.0;
	ball.SpeedY = 0.0;
	ball.BoostX = 0.0;
	ball.BoostY = 0.0;
	ball.directionX = NONE_X;
	ball.directionY = NONE_Y;
}

void DrawBall(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	BOOL result = DrawBitmap(hdc, ball.X - ball.Radius, ball.Y - ball.Radius, hBmpBall);
	if (result == FALSE) {
		MessageBox(NULL,
			_T("Call to DrawBitmap failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
	}
	EndPaint(hWnd, &ps);
}

BOOL DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
	HBITMAP hBmp, hBmpOld;
	HDC hMemDC;
	BITMAP bmp;
	POINT ptSize, ptOrg;

	hMemDC = CreateCompatibleDC(hDC); // creating memory context, that is compatible with hDC
	//Чтобы приложения могли размещать выходные данные в памяти, а не отправлять их на фактическое устройство
	if (hMemDC == NULL) {
		return FALSE;
	}


	hBmpOld = (HBITMAP)SelectObject(hMemDC, hBitmap); // select image into the context	
	// this function returns ID of the 
	//BMP that was loaded into the memory context earlier
	if (!hBmpOld) {
		return FALSE;
	}

	SetMapMode(hMemDC, GetMapMode(hDC)); // synchronizing of the memory context and showing context
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);

	ptSize.x = bmp.bmWidth;
	ptSize.y = bmp.bmHeight;
	DPtoLP(hDC, &ptSize, 1); // convert units into logical units

	ptOrg.x = 0;
	ptOrg.y = 0;
	DPtoLP(hDC, &ptSize, 1);

	TransparentBlt(hDC, x, y, ptSize.x, ptSize.y, hMemDC, ptOrg.x, ptOrg.y, ptSize.x, ptSize.y, RGB(255, 255, 255));

	SelectObject(hMemDC, hBmpOld); // restore memory context
	DeleteObject(hMemDC);
}

void RecalculateBallSpeed()
{
	if (ball.directionX != NONE_X) {
		ball.SpeedX = ball.SpeedX - ball.BoostX;
		if (ball.SpeedX > (-1) * ALLOWED_FAULT && ball.SpeedX < ALLOWED_FAULT) {
			ball.SpeedX = 0.0;
			ball.BoostX = 0.0;
			ball.directionX = NONE_X;
		}
	}

	if (ball.directionY != NONE_Y) {
		ball.SpeedY = ball.SpeedY - ball.BoostY;
		if (ball.SpeedY > (-1) * ALLOWED_FAULT && ball.SpeedY < ALLOWED_FAULT) {
			ball.SpeedY = 0.0;
			ball.BoostY = 0.0;
			ball.directionY = NONE_Y;
		}
	}
}

void RecalculateBallPosition()
{
	switch (ball.directionX) {
	case LEFT:
		ball.X -= ball.SpeedX - ball.BoostX / 2;
		if (LeftHitten()) { ball.directionX = RIGHT; }
		break;
	case RIGHT:
		ball.X += ball.SpeedX - ball.BoostX / 2;
		if (RightHitten()) { ball.directionX = LEFT; }
		break;
	case NONE_X:
		break;
	}

	switch (ball.directionY) {
	case UP:
		ball.Y -= ball.SpeedY - ball.BoostY / 2;
		if (TopHitten()) { 
            ball.directionY = NONE_Y;
            /*ball.directionY = DOWN; */}
		break;
	case NONE_Y:
        if (ball.Y < WND_HEIGHT - 65) {
            Gravity();
        }
		break;
	}
   
}

void SetUpLeftHit()
{
	ball.directionX = LEFT;
	ball.SpeedX = START_SPEED;
	ball.BoostX = BOOST;
}

void SetUpRightHit()
{
	ball.directionX = RIGHT;
	ball.SpeedX = START_SPEED;
	ball.BoostX = BOOST;
}

void SetUpUpHit()
{
	ball.directionY = UP;
	ball.SpeedY = START_SPEED;
	ball.BoostY = BOOST;
}

BOOL LeftHitten()
{
	if (ball.X - ball.Radius <= 0) { return TRUE; }
	else { return FALSE; }
}

BOOL RightHitten()
{
	if (ball.X + ball.Radius >= WND_WIDTH) { return TRUE; }
	else { return FALSE; }
}

BOOL TopHitten()
{
	if (ball.Y - ball.Radius <= 0) { return TRUE; }
	else { return FALSE; }
}

BOOL BottomHitten()
{
	if (ball.Y + ball.Radius >= WND_HEIGHT - 20) { return TRUE; }
	else { return FALSE; }

}