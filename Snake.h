#ifndef SNAKE_H
#define SNAKE_H

#include <Windows.h>
#include <stdbool.h>

typedef struct Snake Snake;

typedef struct Node Node;

/**
 * CreateSnake - Creates the snake.
 * Returns: A pointer to a snake.
 */
Snake* CreateSnake(RECT head);

/**
 * UpdateSnake - Updates the snake by adding a new rect for where the head is moving and removes the tail
 * (if the snake isn't eating the food this move).
 * 
 * Returns: 1 if the snake is coliding with itself, 0 otherwise.
 */
int UpdateSnake(Snake* snake, RECT newHeadRect, bool increaseLenght);

/**
 * PaintSnake - Paints the snake.
 */
void PaintSnake(Snake* snake, HWND hWnd, HDC hDc, HBRUSH headColor, HBRUSH bodyColor);

/**
 * GetSnakeHead - Gets the head rectangle for the snake.
 * Returns: A pointer to a rectangle.
 */
RECT* GetSnakeHead(Snake* snake);

/** 
 * KillSnake - Frees memory allocated by the snake.
 */
void KillSnake(Snake* snake);

#endif