#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;
CRITICAL_SECTION cs;																	//Критическая секция

DWORD WINAPI print(LPVOID lpParam) {

	char name[10];																		//Название сигнала для завершения работы
	sprintf(name, "%d", GetCurrentThreadId());											//Название сигнала = id потока

	HANDLE waitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, name);								//Открытие сигнала

	string str = "Thread id: ";
	str += name;
	str += "\n";
	while (!WaitForSingleObject(waitEvent, 0) == WAIT_OBJECT_0) {						//Проверка на получение сигнала о завершении работы

		EnterCriticalSection(&cs);														//Вход в критическую секцию

		for (char c : str) {
			cout << c;
			cout.flush();
			Sleep(10);
		}

		Sleep(100);
		LeaveCriticalSection(&cs);														//Выход из критический секции
		Sleep(100);
	}

	return 0;
}

int main()
{
	char buf[10];

	cout << "\t\t<***********************************>" << endl;
	cout << "\t\t\tCreate: +" << endl;
	cout << "\t\t\tKill: -" << endl;
	cout << "\t\t\tExit: q" << endl;
	cout << "\t\t<***********************************>" << endl;

	InitializeCriticalSection(&cs);														//Инициализация критической секции

	DWORD lastThreadId;																	//Id последнего созданного потока
	HANDLE lastThread;																	//Handle последнего созданного потока
	HANDLE waitEvent;																	//Handle сигнала для последнего созданного потока
	vector<HANDLE> threadsVect;															//Вектор Handle потоков
	vector<HANDLE> eventsVect;															//Вектор Handle сигналов

	while (true) {
		switch (_getch())
		{
		case('+') :

			lastThread = CreateThread(NULL, 0, &print, NULL, 0, &lastThreadId);
			threadsVect.push_back(lastThread);

			sprintf(buf, "%d", lastThreadId);
			waitEvent = CreateEvent(NULL, TRUE, FALSE, buf);
			eventsVect.push_back(waitEvent);
			break;

		case('-') :

			if (!threadsVect.size()) break;
			SetEvent(eventsVect[eventsVect.size() - 1]);										//Сигнал процессу
			CloseHandle(eventsVect[eventsVect.size() - 1]);
			eventsVect.pop_back();
			threadsVect.pop_back();
			break;

		case('q') :
			while (threadsVect.size()) {
				SetEvent(eventsVect[eventsVect.size() - 1]);											//Сигнал процессу
				CloseHandle(eventsVect[eventsVect.size() - 1]);
				WaitForSingleObject(threadsVect[threadsVect.size() - 1], INFINITE);						//Ожидание завершения потока
				eventsVect.pop_back();
				threadsVect.pop_back();
			}
				  return 0;
				  break;
		}

	}
}