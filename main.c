#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Snake.h"

// Some easy to modify constants
#define WIDTH_OF_SNAKE 50
#define FRAMES_PER_SECONDS 4
#define GAME_BOX_SIZE 750

// Declarations for methods
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnSize(HWND hwnd, UINT flag, int width, int height);
RECT newFoodLocation();
void GameLoop();

// Game variables
int xPos, yPos, xDir, yDir, clientWidth, clientHeight;
int gameOver = 0;
int score = 0;

// Holders for game objects
Snake* snake;
RECT foodRect;

// The rectangle that will contain the game
RECT GAME_BOX = {WIDTH_OF_SNAKE, WIDTH_OF_SNAKE, WIDTH_OF_SNAKE + GAME_BOX_SIZE, WIDTH_OF_SNAKE + GAME_BOX_SIZE};

// Columns and rows
const int COLS = GAME_BOX_SIZE / WIDTH_OF_SNAKE;
const int ROWS = GAME_BOX_SIZE / WIDTH_OF_SNAKE;

/**
 * wWinMain - Entrypoint for this program.
 * Creates the window and configures it.
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Sets timeseed for rand
	srand(time(NULL));

	// Sets some starter positions and directions
	xPos = GAME_BOX.left + 50;
	yPos = GAME_BOX.top + 50;
	xDir = yDir = 0;

	// Calculates the windowsize
	clientWidth = clientHeight = GAME_BOX_SIZE + 2.5 * WIDTH_OF_SNAKE;

	// Creates the food and the snake
	foodRect = newFoodLocation();
	snake = CreateSnake((RECT) {xPos, yPos, xPos + 50, yPos + 50});

	const char CLASS_NAME[] = "Test Window";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);
	
	// Configures a non-resizablke window
	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"Test Window i've made",
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,

		CW_USEDEFAULT, CW_USEDEFAULT, clientWidth, clientHeight,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	// Check if window creation failed
	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Create messages and push messages
	MSG msg = {};
	while (GetMessageA(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg); // Translates keyinputs to messages
		DispatchMessageA(&msg);
	}
	
	return 0;
}

/**
 * WinProc - Handles everything related to window messages
 */
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);  // Get the entire client area

	const int TIMER_ID = 1;

	switch (uMsg)
	{
		case WM_CREATE:
			double timer_interval = 1000.0 / FRAMES_PER_SECONDS;
			
			// Starting a timed loop
			SetTimer(hWnd, TIMER_ID, timer_interval, NULL);
			break;

		case WM_TIMER:
			if (wParam == TIMER_ID) 
			{
				InvalidateRect(hWnd, NULL, TRUE); // Force redraw of window
				GameLoop();
			}
			break;

		case WM_DESTROY:
			// Kill the timer
			KillTimer(hWnd, TIMER_ID);
			
			// Kill the snake
			KillSnake(snake);

			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
			PAINTSTRUCT ps;
			HBRUSH greenBrush = CreateSolidBrush(RGB(0,255,0));
			HBRUSH darkGreenBrush = CreateSolidBrush(RGB(0,50,50));
			HBRUSH blackBrush = CreateSolidBrush(RGB(0,0,0));
			HBRUSH redBrush = CreateSolidBrush(RGB(255,0,0));

			HDC hDc = BeginPaint(hWnd, &ps);

			FillRect(hDc, &ps.rcPaint, blackBrush);
			FillRect(hDc, &GAME_BOX, (HBRUSH) WHITE_BRUSH);
			PaintSnake(snake, hWnd, hDc, darkGreenBrush, greenBrush);
			FillRect(hDc, &foodRect, redBrush);
			
			// Draw the text at the bottom
			char textBuff[200];
			sprintf(textBuff, "Score: %d", score);
			DrawTextA(hDc, textBuff, -1, &clientRect, DT_SINGLELINE | DT_CENTER);
			
			if (gameOver)			
				DrawTextA(hDc, "GAME OVER!", -1, &GAME_BOX, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			
			EndPaint(hWnd, &ps);
			DeleteObject(greenBrush);
			DeleteObject(darkGreenBrush);
			DeleteObject(blackBrush);
			DeleteObject(redBrush);
			break;
		
		case WM_KEYDOWN:
			if (gameOver)
				break;

			switch (wParam)
			{
				case VK_LEFT:
					xDir = -WIDTH_OF_SNAKE;
					yDir = 0;
					break;
				
				case VK_RIGHT:
					xDir = WIDTH_OF_SNAKE;
					yDir = 0;
					break;
				
				case VK_UP:
					yDir = -WIDTH_OF_SNAKE;
					xDir = 0;
					break;

				case VK_DOWN:
					yDir = WIDTH_OF_SNAKE;
					xDir = 0;
					break;
			}

			break;

		case WM_GETMINMAXINFO:
			// Forces window size
			MINMAXINFO* pMinMaxInfo = (MINMAXINFO*) lParam;
			
			pMinMaxInfo->ptMinTrackSize.x = pMinMaxInfo->ptMaxTrackSize.x = clientWidth;
			pMinMaxInfo->ptMinTrackSize.y = pMinMaxInfo->ptMaxTrackSize.y = clientHeight;
			
			break;
	}

	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

void OnSize(HWND hWnd, UINT flag, int width, int height)
{
	InvalidateRect(hWnd, NULL, TRUE); 
}

/**
 * IsGameOver checks if game is over.
 * Returns: 1 if game is over else 0.
 */
int IsGameOver() 
{
	if (xPos < GAME_BOX.left || xPos >= GAME_BOX.right)
		return 1;
	
	if (yPos < GAME_BOX.top || yPos >= GAME_BOX.bottom)
		return 1;
	
	return 0;
}

/**
 * Generates a new location for the food
 * Returns a Rectangle (RECT).
 */
RECT newFoodLocation() {
	int col = 1 + rand() % COLS;
	int row = 1 + rand() % ROWS;

	RECT rect = {col * WIDTH_OF_SNAKE, row * WIDTH_OF_SNAKE, 
				col * WIDTH_OF_SNAKE + WIDTH_OF_SNAKE, row * WIDTH_OF_SNAKE + WIDTH_OF_SNAKE};

	return rect;
}

/**
 * GameLoop houses the game loop.
 */
void GameLoop() 
{
	// Checks if the game is over
	if (IsGameOver() || gameOver) 
	{
		gameOver = 1;
		xDir = yDir = 0;
		return;
	}
	
	// Update snake head pos
	xPos += xDir;
	yPos += yDir;

	// Placeholder rect for the snakes head.
	RECT rect = {xPos, yPos, xPos + WIDTH_OF_SNAKE, yPos + WIDTH_OF_SNAKE};

	// Checks if the snake is eating the food (head on food)
	bool eating = EqualRect(GetSnakeHead(snake), &foodRect);

	// Increase the score and spawn new food
	if (eating)
	{
		foodRect = newFoodLocation();
		score += 1;
	}

	// Updates the snake. 
	if (UpdateSnake(snake, rect, eating))
		gameOver = 1;
}


