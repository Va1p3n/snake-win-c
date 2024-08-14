#include <Windows.h>
#include <stdbool.h>

#include "Snake.h"

typedef struct Snake
{
	Node* head;
} Snake;

typedef struct Node
{
	RECT bodyRect;
	struct Node* next;
} Node;

Node* createNewSnakeHead(RECT rect)
{
	Node* pNode = malloc(sizeof(Node));
	pNode->bodyRect = rect;

	return pNode;
}

Snake* CreateSnake(RECT head)
{
	Snake* pSnake = malloc(sizeof(Snake));
	pSnake->head = malloc(sizeof(Node));

	pSnake->head->bodyRect = head;
	pSnake->head->next = NULL;

	return pSnake;
}

int UpdateSnake(Snake* snake, RECT newHeadRect, bool increaseLenght) 
{
	Node* newHead = createNewSnakeHead(newHeadRect);
	newHead->next = snake->head;

	snake->head = newHead;

	if (!increaseLenght)
	{
		Node* currentLookedAtNode = snake->head;

		while (currentLookedAtNode->next->next != NULL)
		{
			currentLookedAtNode = currentLookedAtNode->next;
		}
		
		Node* nodeToRemove = currentLookedAtNode->next;
		currentLookedAtNode->next = NULL;

		free(nodeToRemove);
	}

	Node* masterCompNode = snake->head;
	Node* slaveCompNode = masterCompNode->next;

	while (masterCompNode != NULL)
	{
		while (slaveCompNode != NULL)
		{
			if (EqualRect(&masterCompNode->bodyRect, &slaveCompNode->bodyRect))
			{
				return 1;
			}

			slaveCompNode = slaveCompNode->next;
		}

		masterCompNode = masterCompNode->next;
	}
	
	return 0;
}

void PaintSnake(Snake* snake, HWND hWnd, HDC hDc, HBRUSH headColor, HBRUSH bodyColor)
{
	Node* nodeToPaint = snake->head;
	bool paintedHead = false;

	while (nodeToPaint != NULL)
	{
		if (!paintedHead)
		{
			FillRect(hDc, &nodeToPaint->bodyRect, headColor);
			paintedHead = true;
			continue;
		}

		FillRect(hDc, &nodeToPaint->bodyRect, bodyColor);

		nodeToPaint = nodeToPaint->next;
	}
	
}

RECT* GetSnakeHead(Snake* snake)
{
	return &snake->head->bodyRect;
}

void KillSnake(Snake* snake)
{
	Node* nodeToRemove = snake->head;

	while (nodeToRemove != NULL)
	{
		Node* processingNode = nodeToRemove;
		nodeToRemove = nodeToRemove->next;

		free(processingNode);
	}
	
	free(snake);
}