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
		InitializeMario(hWnd);
        InitializeBlock(hWnd);
		SetTimer(hWnd, timer, 1, NULL);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0); //Помещает в очередь сообщений сообщение WM_quit
		//и тогда GetMessage вернет FALSE
		break;
	case WM_PAINT:
		DrawScene(hWnd);
       
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
	hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBmpblock1 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	int Error = GetLastError();

}

void Gravity() {
	mario.Y = mario.Y + GRAVITATION;
}
void InitializeMario(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hBmpMario, sizeof(BITMAP), (LPSTR)&bmp);
	mario.X = WND_WIDTH / 2;
	mario.Y = WND_HEIGHT / 2;
	mario.Radius = bmp.bmWidth / 2;//24
	mario.SpeedX = 0.0;
	mario.SpeedY = 0.0;
	mario.BoostX = 0.0;
	mario.BoostY = 0.0;
	mario.directionX = NONE_X;
	mario.directionY = NONE_Y;
}

void InitializeBlock(HWND hWnd)
{
    BITMAP bmp;
    GetObject(hBmpblock1, sizeof(BITMAP), (LPSTR)&bmp);
    block1.X = WND_WIDTH / 2 + 350;
    block1.Y = WND_HEIGHT - 65;
    block1.Radius = bmp.bmWidth / 2;
    block1.SpeedX = 0.0;
    block1.SpeedY = 0.0;
    block1.BoostX = 0.0;
    block1.BoostY = 0.0;
    block1.directionX = NONE_X;
    block1.directionY = NONE_Y;
}

void DrawScene(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	BOOL result = DrawBitmap(hdc, mario.X - mario.Radius, mario.Y - mario.Radius, hBmpMario);
    BOOL result2 = DrawBitmap(hdc, block1.X - block1.Radius, block1.Y - block1.Radius, hBmpblock1);
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
	if (mario.directionX != NONE_X) {
		mario.SpeedX = mario.SpeedX - mario.BoostX;
		if (mario.SpeedX > (-1) * ALLOWED_FAULT && mario.SpeedX < ALLOWED_FAULT) {
			mario.SpeedX = 0.0;
			mario.BoostX = 0.0;
			mario.directionX = NONE_X;
		}
	}

	if (mario.directionY != NONE_Y) {
		mario.SpeedY = mario.SpeedY - mario.BoostY;
		if (mario.SpeedY > (-1) * ALLOWED_FAULT && mario.SpeedY < ALLOWED_FAULT) {
			mario.SpeedY = 0.0;
			mario.BoostY = 0.0;
			mario.directionY = NONE_Y;
		}
	}
}

void RecalculateBallPosition()
{
   
	switch (mario.directionX) {
	case LEFT:
		mario.X -= mario.SpeedX - mario.BoostX / 2;
		if (LeftHitten()) { mario.directionX = RIGHT; }
		break;
        
	case RIGHT:
		mario.X += mario.SpeedX - mario.BoostX / 2;
		if (RightHitten()) { mario.directionX = LEFT; }
		break;
	case NONE_X:
		break;
	}

	switch (mario.directionY) {
	case UP:
		mario.Y -= mario.SpeedY - mario.BoostY / 2;
		if (TopHitten()) { 
            mario.directionY = NONE_Y;
            /*ball.directionY = DOWN; */}
		break;
	case NONE_Y:
        if (mario.Y < WND_HEIGHT - 65) {
            Gravity();
        }
		break;
	}
    Collision(mario, block1);
    
  
   
}

void SetUpLeftHit()
{
	mario.directionX = LEFT;
	mario.SpeedX = START_SPEED;
	mario.BoostX = BOOST;
}

void SetUpRightHit()
{
	mario.directionX = RIGHT;
	mario.SpeedX = START_SPEED;
	mario.BoostX = BOOST;
}

void SetUpUpHit()
{
	mario.directionY = UP;
	mario.SpeedY = START_SPEED;
	mario.BoostY = BOOST;
}

BOOL LeftHitten()
{
	if (mario.X - mario.Radius <= 0) { return TRUE; }
	else { return FALSE; }
}

BOOL RightHitten()
{
	if (mario.X + mario.Radius >= WND_WIDTH) { return TRUE; }
	else { return FALSE; }
}

BOOL TopHitten()
{
	if (mario.Y - mario.Radius <= 0) { return TRUE; }
	else { return FALSE; }
}

BOOL BottomHitten()
{
	if (mario.Y + mario.Radius >= WND_HEIGHT - 20) { return TRUE; }
	else { return FALSE; }

}

void Collision(Mario& mario, Block& block)
{
	//HORIZONTAL
	int HhorizontalDistance = mario.X - block.X;
	int HverticalDistance = (mario.Y - mario.Radius) - (block.Y + block.Radius);
	int HAverticalDistance = abs(HverticalDistance);
	int HAhorizontalDistance = abs(HhorizontalDistance);

	//VERTICAL
	int VhorizontalDistance = (mario.X - mario.Radius) - (block.X - block.Radius);
	int VverticalDistance =  mario.Y - block.Y;
	int VAverticalDistance = abs(VverticalDistance);
	int VAhorizontalDistance = abs(VhorizontalDistance);

	switch (mario.directionX) {
	case (LEFT):
		if ((HAhorizontalDistance <= BLOCKACCURACY) && (HAverticalDistance <= BLOCKACCURACY)) {
			mario.directionX = NONE_X;
			mario.X += 10;
		}
		break;
	case (RIGHT):
		if ((HAhorizontalDistance <= BLOCKACCURACY) && (HAverticalDistance <= BLOCKACCURACY)) {
			mario.directionX = NONE_X;
			mario.X -= 10;
		}
	  default:
		  break;
	  }

	switch (mario.directionY) {
	case (UP):
		if ((VAverticalDistance <= BLOCKACCURACY) && (VAhorizontalDistance <= BLOCKACCURACY)) {
			mario.directionX = NONE_X;
			mario.X += 10;
		}
		break;
	case (NONE_Y):
		if ((VAverticalDistance <= BLOCKACCURACY) && (VAhorizontalDistance <= BLOCKACCURACY)) {
			mario.Y -= 10;
		}
		break;
	default:
		break;
	}
}