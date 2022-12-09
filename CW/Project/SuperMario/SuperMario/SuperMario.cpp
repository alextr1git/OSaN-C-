#include "SuperMario.h"
#include "resource.h"

int X, Y = 0;
int gravityX = 0;
int endflag = 0;
int waitflag = 0;
int wait = 1001;
int gravityLevel = BASICGRAVITY;
int spaceCheck = 0;
int spacePos = 0;
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
		InitializeClouds(hWnd);
		InitializeGoomba(hWnd);
		SetTimer(hWnd, timer, 1, NULL);
		spacePos = mario.Y;
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
		if (waitflag == 1)
			wait++;
		RecalculateMarioSpeed();
		RecalculateMarioPosition();
		RecalculateGoombaPosition();
		InvalidateRect(hWnd, NULL, TRUE);//обновить область рисования
		switch (endflag) 
		{
		case 1:
			KillTimer(hWnd, 1);
			MessageBox(NULL,
				_T("Game over!"),
				_T("Congrats!"),
				NULL);
			break;
		case 2:
			KillTimer(hWnd, 1);
			MessageBox(NULL,
				_T("You have passed the level!"),
				_T("Congrats!"),
				NULL);
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			SetUpLeftHit();
			hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\leftMario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		case VK_RIGHT:
			SetUpRightHit();
			hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\rightMario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			break;
		case VK_SPACE:
			if (spaceCheck < 2) {
				spaceCheck++;
				SetUpUpHit();
			}
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
	hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\rightMario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBmpblock1 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	hBmpblocks1 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBmpblocks2 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBmpblocks3 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBmpblocks4 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBmpblocks5 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBmpblocks6 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBmpblocks7 = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hClouds = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\clouds.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hGoomba = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\goomba.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	int Error = GetLastError();

}

void Gravity() {
	if (mario.Y < gravityLevel)
		mario.Y = mario.Y + GRAVITATION;
}

void GoombaGravity() {
	if (goomba.Y < WND_HEIGHT - 85)
		goomba.Y = goomba.Y + 1;
}
void InitializeMario(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hBmpMario, sizeof(BITMAP), (LPSTR)&bmp);
	mario.X = 50;
	mario.Y = 500;
	mario.Radius = bmp.bmWidth / 2;//24
	mario.SpeedX = 0.0;
	mario.SpeedY = 0.0;
	mario.BoostX = 0.0;
	mario.BoostY = 0.0;
	mario.directionX = NONE_X;
	mario.directionY = NONE_Y;
}

void InitializeGoomba(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hGoomba, sizeof(BITMAP), (LPSTR)&bmp);
	goomba.X = 1400;
	goomba.Y = 500;
	goomba.Radius = bmp.bmWidth / 2;//24
	goomba.SpeedX = 0.0;
	goomba.SpeedY = 0.0;
	goomba.BoostX = 0.0;
	goomba.BoostY = 0.0;
	goomba.directionX = NONE_X;
	goomba.directionY = NONE_Y;
}

void InitializeClouds(HWND hWnd)
{
	BITMAP bmp;
	GetObject(hBmpMario, sizeof(BITMAP), (LPSTR)&bmp);
	clouds.X = 0;
	clouds.Y = 0;
	clouds.Radius = 0;
	clouds.SpeedX = 0.0;
	clouds.SpeedY = 0.0;
	clouds.BoostX = 0.0;
	clouds.BoostY = 0.0;
	clouds.directionX = NONE_X;
	clouds.directionY = NONE_Y;
}

void InitializeBlock(HWND hWnd)
{
    BITMAP bmp;

	GetObject(hBmpblocks1, sizeof(BITMAP), (LPSTR)&bmp);
	GetObject(hBmpblocks2, sizeof(BITMAP), (LPSTR)&bmp);
	GetObject(hBmpblocks3, sizeof(BITMAP), (LPSTR)&bmp);
	GetObject(hBmpblocks4, sizeof(BITMAP), (LPSTR)&bmp);
	GetObject(hBmpblocks5, sizeof(BITMAP), (LPSTR)&bmp);
	GetObject(hBmpblocks6, sizeof(BITMAP), (LPSTR)&bmp);
	GetObject(hBmpblocks7, sizeof(BITMAP), (LPSTR)&bmp);

	GetObject(hClouds, sizeof(BITMAP), (LPSTR)&bmp);

	InitializeBlocksArray(hBmpblocks1, 0);
	InitializeBlocksArray(hBmpblocks2, 1);
	InitializeBlocksArray(hBmpblocks3, 2);
	InitializeBlocksArray(hBmpblocks4, 3);
	InitializeBlocksArray(hBmpblocks5, 4);
	InitializeBlocksArray(hBmpblocks6, 5);
	InitializeBlocksArray(hBmpblocks7, 6);

}
void InitializeBlocksArray(HBITMAP hBmpblock, int i) {
	BITMAP bmp;
	switch (i) {
	case 0:
		blocks[i].X = 225;
		blocks[i].Y = 450;
		break;
	case 1:
		blocks[i].X = 425;
		blocks[i].Y = 300;
		break;
	case 2:
		blocks[i].X = 525;
		blocks[i].Y = 450;
		break;
	case 3:
		blocks[i].X = 575;
		blocks[i].Y = 450;
		break;
	case 4:
		blocks[i].X = 625;
		blocks[i].Y = 450;
		break;
	case 5:
		blocks[i].X = 825;
		blocks[i].Y = 400;
		break;
	case 6:
		blocks[i].X = 1200;
		blocks[i].Y = 535;
		break;
	}

	blocks[i].Radius = 24;
	blocks[i].SpeedX = 0.0;
	blocks[i].SpeedY = 0.0;
	blocks[i].BoostX = 0.0;
	blocks[i].BoostY = 0.0;
	blocks[i].directionX = NONE_X;
	blocks[i].directionY = NONE_Y;
}

void InitializeBlocksArrayRandom(HBITMAP hBmpblock, int i) {
	srand(time(NULL));
	int randomNumberY = rand() % 200 + 300;
	int randomNumberX = rand() % 10 + 1;
	BITMAP bmp;
	blocks[i].X = 100 + i * 240 + randomNumberX * 10;
	blocks[i].Y = randomNumberY;
	blocks[i].Radius = 24;
	blocks[i].SpeedX = 0.0;
	blocks[i].SpeedY = 0.0;
	blocks[i].BoostX = 0.0;
	blocks[i].BoostY = 0.0;
	blocks[i].directionX = NONE_X;
	blocks[i].directionY = NONE_Y;
}

void DrawScene(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	BOOL result = DrawBitmap(hdc, mario.X - mario.Radius, mario.Y - mario.Radius, hBmpMario);

	BOOL results1 = DrawBitmap(hdc, blocks[0].X - blocks[0].Radius, blocks[0].Y - blocks[0].Radius, hBmpblocks1);
	BOOL results2 = DrawBitmap(hdc, blocks[1].X - blocks[1].Radius, blocks[1].Y - blocks[1].Radius, hBmpblocks2);
	BOOL results3 = DrawBitmap(hdc, blocks[2].X - blocks[2].Radius, blocks[2].Y - blocks[2].Radius, hBmpblocks3);
	BOOL results4 = DrawBitmap(hdc, blocks[3].X - blocks[3].Radius, blocks[3].Y - blocks[3].Radius, hBmpblocks4);
	BOOL results5 = DrawBitmap(hdc, blocks[4].X - blocks[4].Radius, blocks[4].Y - blocks[4].Radius, hBmpblocks5);
	BOOL results6 = DrawBitmap(hdc, blocks[5].X - blocks[5].Radius, blocks[5].Y - blocks[5].Radius, hBmpblocks6);
	BOOL results7 = DrawBitmap(hdc, blocks[6].X - blocks[6].Radius, blocks[6].Y - blocks[6].Radius, hBmpblocks7);

	BOOL resultC = DrawBitmap(hdc, clouds.X, clouds.Y, hClouds);
	BOOL resultGoomba = DrawBitmap(hdc, goomba.X, goomba.Y, hGoomba);

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

void RecalculateMarioSpeed()
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
void RecalculateGoombaSpeed()
{
	if (goomba.directionX != NONE_X) {
		goomba.SpeedX = goomba.SpeedX - goomba.BoostX;
		if (goomba.SpeedX > (-1) * ALLOWED_FAULT && goomba.SpeedX < ALLOWED_FAULT) {
			goomba.SpeedX = 0.0;
			goomba.BoostX = 0.0;
			goomba.directionX = NONE_X;
		}
	}

	if (goomba.directionY != NONE_Y) {
		goomba.SpeedY = goomba.SpeedY - goomba.BoostY;
		if (goomba.SpeedY > (-1) * ALLOWED_FAULT && goomba.SpeedY < ALLOWED_FAULT) {
			goomba.SpeedY = 0.0;
			goomba.BoostY = 0.0;
			goomba.directionY = NONE_Y;
		}
	}
}
void RecalculateGoombaPosition()
{
	if (wait > 100)
	{
		hGoomba = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\goomba.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		waitflag == 0;
			if (goomba.X > mario.X) {
				goomba.X -= 3;
			}
			else if (goomba.X < mario.X)
			{
				goomba.X += 3;
			}
			int SumX = goomba.X - mario.X;
			int ASumX = abs(SumX);
			int SumY = goomba.Y - mario.Y;
			int ASumY = abs(SumY);
			if (ASumX <= 48 && ASumY <= 48 && mario.Y >= goomba.Y) {
				endflag = 1;
			}
			if (ASumX <= 48 && ASumY <= 48 && mario.Y < goomba.Y) {
				waitflag = 1;
				wait = 0;
				hGoomba = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\goombaDead.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				goomba.X = 1350;
				goomba.Y = 500;
			}

		for (int i = 0; i < AMOUNTOFBLOCKS; i++) {
			Collision(goomba, blocks[i]);
		}
		GoombaGravity();
	}
}

void RecalculateMarioPosition()
{
	if (mario.Y  >= spacePos)
		spaceCheck = 0;

	if (gravityLevel != BASICGRAVITY) {
		int sumGravity = gravityX - mario.X;
		int ASG = abs(sumGravity);
		if (ASG > 47) {
			gravityLevel = BASICGRAVITY;
			spacePos = 500;
		}
		
	}
	switch (mario.directionX) {
	case LEFT:	
		mario.X -= mario.SpeedX - mario.BoostX / 2;
		hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\leftMario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (LeftHitten()) { mario.directionX = RIGHT; }
		break;
        
	case RIGHT:
		mario.X += mario.SpeedX - mario.BoostX / 2;
		if (RightHitten()) {
			mario.directionX = LEFT;
			endflag = 2;

		}
		break;
	case NONE_X:
		hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\rightMario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	}

	switch (mario.directionY) {
	case UP:
		mario.Y -= mario.SpeedY - mario.BoostY / 2;
		if (TopHitten()) {
			mario.directionY = NONE_Y;
			
		}
		hBmpMario = (HBITMAP)LoadImage(hInst, L"C:\\Users\\User\\Desktop\\res\\jumpMario.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case NONE_Y:
        if (mario.Y < WND_HEIGHT - 65) {
            Gravity();
        }
		
		break;
	}
	for (int i = 0; i < AMOUNTOFBLOCKS; i++) {
		Collision(mario, blocks[i]);
	}    
  
   
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
	mario.SpeedY = 10;
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
	int HverticalDistance = (mario.Y) - (block.Y);
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
			mario.Y -= 5;
			gravityLevel = mario.Y;
			gravityX = block.X;
			spacePos = mario.Y;
	
		}
		
		break;
	default:
		break;
	}
}