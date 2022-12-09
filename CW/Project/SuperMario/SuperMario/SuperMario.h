﻿/*#pragma once
#pragma comment( lib, "Msimg32.lib" )

#include "resource.h"
#include "wingdi.h"
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>


enum DirectionX { LEFT, RIGHT, NONE_X };
enum DirectionY { UP, DOWN, NONE_Y };

typedef struct ball {
	float X, Y, Radius;
	float SpeedX, SpeedY;
	float BoostX, BoostY;
	DirectionX directionX;
	DirectionY directionY;
} Ball;

Ball ball;
HBITMAP hBmpBall;

int timer = 1;

constexpr auto START_SPEED = 20.0;
constexpr auto BOOST = 0.25;
constexpr auto ALLOWED_FAULT = 1.0;

constexpr auto WND_WIDTH = 1000.0;
constexpr auto WND_HEIGHT = 600.0;

void LoadResources();
void InitializeBall(HWND);


void DrawBall(HWND);
BOOL DrawBitmap(HDC hDc, int x, int y, HBITMAP hBitmap);

BOOL LeftHitten();
BOOL RightHitten();
BOOL TopHitten();
BOOL BottomHitten();

void Gravity();
void SetUpLeftHit();
void SetUpRightHit();
void SetUpUpHit();
void SetUpDownHit();

void RecalculateBallSpeed();
void RecalculateBallPosition();
*/
#pragma once
#pragma comment( lib, "Msimg32.lib" )

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>


// User types
enum DirectionX { LEFT, RIGHT, NONE_X };
enum DirectionY { UP, NONE_Y };

typedef struct ball {
	float X, Y, Radius;
	float SpeedX, SpeedY;
	float BoostX, BoostY;
	DirectionX directionX;
	DirectionY directionY;
} Ball;


// Global variables
static TCHAR szWindowClass[] = _T("TheBall");
static TCHAR szTitle[] = _T("The Ball");
constexpr auto WND_WIDTH = 1550.0;
constexpr auto WND_HEIGHT = 600.0;
HINSTANCE hInst;

Ball ball;
int timer = 1;
HBITMAP hBmpBall;

constexpr auto START_SPEED = 7.0;
constexpr auto BOOST = 0.3;
constexpr auto ALLOWED_FAULT = 1.0;
constexpr auto GRAVITATION = 3;

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

void LoadResources();
void InitializeBall(HWND);

void DrawBall(HWND);
BOOL DrawBitmap(HDC hDc, int x, int y, HBITMAP hBitmap);

void RecalculateBallSpeed();
void RecalculateBallPosition();
