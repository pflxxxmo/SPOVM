#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

using namespace std;
CRITICAL_SECTION cs;																	//����������� ������

DWORD WINAPI print(LPVOID lpParam) {

	char name[10];																		//�������� ������� ��� ���������� ������
	sprintf(name, "%d", GetCurrentThreadId());											//�������� ������� = id ������

	HANDLE waitEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, name);								//�������� �������

	string str = "Thread id: ";
	str += name;
	str += "\n";
	while (!WaitForSingleObject(waitEvent, 0) == WAIT_OBJECT_0) {						//�������� �� ��������� ������� � ���������� ������

		EnterCriticalSection(&cs);														//���� � ����������� ������

		for (char c : str) {
			cout << c;
			cout.flush();
			Sleep(10);
		}

		Sleep(100);
		LeaveCriticalSection(&cs);														//����� �� ����������� ������
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

	InitializeCriticalSection(&cs);														//������������� ����������� ������

	DWORD lastThreadId;																	//Id ���������� ���������� ������
	HANDLE lastThread;																	//Handle ���������� ���������� ������
	HANDLE waitEvent;																	//Handle ������� ��� ���������� ���������� ������
	vector<HANDLE> threadsVect;															//������ Handle �������
	vector<HANDLE> eventsVect;															//������ Handle ��������

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
			SetEvent(eventsVect[eventsVect.size() - 1]);										//������ ��������
			CloseHandle(eventsVect[eventsVect.size() - 1]);
			eventsVect.pop_back();
			threadsVect.pop_back();
			break;

		case('q') :
			while (threadsVect.size()) {
				SetEvent(eventsVect[eventsVect.size() - 1]);											//������ ��������
				CloseHandle(eventsVect[eventsVect.size() - 1]);
				WaitForSingleObject(threadsVect[threadsVect.size() - 1], INFINITE);						//�������� ���������� ������
				eventsVect.pop_back();
				threadsVect.pop_back();
			}
				  return 0;
				  break;
		}

	}
}