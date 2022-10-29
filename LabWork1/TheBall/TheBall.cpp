#include "TheBall.h"
#include "resource.h"

bool mflag = false;
int X, Y = 0;

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
		if (mflag) {
			POINT pos; RECT rc;
			GetCursorPos(&pos);
			GetClientRect(hWnd, &rc);
			MapWindowPoints(hWnd, GetParent(hWnd), (LPPOINT)&rc, 2);
			ball.X = pos.x - rc.left;
			ball.Y = pos.y - rc.top;
			InvalidateRect(hWnd, NULL, TRUE);
			RecalculateBallPosition();
		}
		if (TopHitten()) {
			//SetUpDownHit();
			//SetUpDownBoundHit();
			ball.Y = 0.0;
			SetUpDownBoundHit();
		
			
		}
		if (BottomHitten()) { ball.directionY = UP; }
		if (LeftHitten()) { ball.directionX = RIGHT; }
		if (RightHitten()) { SetUpLeftHit(); }
		RecalculateBallSpeed();
		RecalculateBallPosition();
			

		/*
		RecalculateBallSpeed();
		RecalculateBallPosition();
		InvalidateRect(hWnd, NULL, TRUE);//обновить область рисования*/
		break;
	case WM_LBUTTONDOWN: // mouse click
		mflag = true;
		break;
	case WM_LBUTTONUP:
		mflag = false;
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
	case WM_MOUSEWHEEL:
		switch (GET_KEYSTATE_WPARAM(wParam)) {
		case MK_SHIFT:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) { SetUpRightHit(); } else { SetUpLeftHit(); }
			break;
		default:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) { SetUpUpHit(); } else { SetUpDownHit(); }
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
	hBmpBall = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
}

void InitializeBall(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hBmpBall, sizeof(BITMAP), (LPSTR)& bmp);
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
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)& bmp);
	
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
		if (LeftHitten()) {	ball.directionX = RIGHT; }
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

void SetUpDownBoundHit() {
	ball.directionY = DOWN;
	ball.SpeedY = BOUND_SPEED;
	ball.BoostY = BOUND_BOOST;
}

BOOL LeftHitten()
{
	if (ball.X - ball.Radius <= 0) { return TRUE; }	else { return FALSE; }
}

BOOL RightHitten()
{
	if (ball.X + ball.Radius >= WND_WIDTH) { return TRUE; }	else { return FALSE; }
}

BOOL TopHitten()
{
	if (ball.Y - ball.Radius <= 0) { return TRUE; }	else { return FALSE; }
}

BOOL BottomHitten()
{
	if (ball.Y + ball.Radius >= WND_HEIGHT) { return TRUE; } else { return FALSE; }
}