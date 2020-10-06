#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>

struct Data
{
	HANDLE semaphore;
	HANDLE channel;
};

void Connect(struct Data*);
void Run(struct Data*);
void Close(struct Data*);
