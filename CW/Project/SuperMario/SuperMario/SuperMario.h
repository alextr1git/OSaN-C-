#pragma once
#pragma comment( lib, "Msimg32.lib" )

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>


// User types
enum DirectionX { LEFT, RIGHT, NONE_X };
enum DirectionY { UP, NONE_Y };

typedef struct mario {
	float X, Y, Radius;
	float SpeedX, SpeedY;
	float BoostX, BoostY;
	DirectionX directionX;
	DirectionY directionY;
} Mario;

typedef struct block {
	float X, Y, Radius;
	float SpeedX, SpeedY;
	float BoostX, BoostY;
	DirectionX directionX;
	DirectionY directionY;
} Block;


// Global variables
static TCHAR szWindowClass[] = _T("Super Mario");
static TCHAR szTitle[] = _T("Super Mario");
constexpr auto WND_WIDTH = 1550.0;
constexpr auto WND_HEIGHT = 600.0;
HINSTANCE hInst;

Mario mario;
Block block1;


int timer = 1;
HBITMAP hBmpMario;
HBITMAP hBmpblock1;

HBITMAP hBmpblocks1;
HBITMAP hBmpblocks2;
HBITMAP hBmpblocks3;
HBITMAP hBmpblocks4;
HBITMAP hBmpblocks5;

constexpr auto START_SPEED = 7.0;
constexpr auto BOOST = 0.3;
constexpr auto ALLOWED_FAULT = 1.0;
constexpr auto GRAVITATION = 5;
constexpr auto BLOCKACCURACY = 50;
constexpr auto AMOUNTOFBLOCKS = 5;

Block blocks[AMOUNTOFBLOCKS];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void SetUpLeftHit();
void SetUpRightHit();
void SetUpUpHit();
void Gravity();

BOOL LeftHitten();
BOOL RightHitten();
BOOL TopHitten();
BOOL BottomHitten();
void Collision(Mario&, Block&);

void LoadResources();
void InitializeMario(HWND);
void InitializeBlock(HWND);
void InitializeBlocksArray(HBITMAP, int);

void DrawScene(HWND);
void DrawBlock(HWND);
BOOL DrawBitmap(HDC hDc, int x, int y, HBITMAP hBitmap);

void RecalculateBallSpeed();
void RecalculateBallPosition();
