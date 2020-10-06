#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>
#include <deque>

constexpr auto ParentPath = "D:\\Visual Studio\\4\\laba2_spovm\\Debug\\child_proc.exe"; //����� ������������� ��������
constexpr auto ChildPath = "D:\\Visual Studio\\4\\laba2_spovm\\Debug\\output.exe"; //����� ��������� ��������


using namespace std;

int main()
{
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);						//��������� ���� � ������������� �����
	int process_counter = 1;										    //������� ��������� ��� ����������� �������� ��� � ����� ������
	HANDLE wait_event = CreateEvent(NULL, true, FALSE, "Wait_event");   //�������� ������� ��� �������������� �������� �������

	STARTUPINFO startupInfoParent;										//��������� STARTUPINFO ��� ������������� ��������
	ZeroMemory(&startupInfoParent, sizeof(STARTUPINFO));				//��������� ��������� ��������� STARTUPINFO
	PROCESS_INFORMATION processInfoParent;								//��������� PROCESS_INFORMATION ��� ������������� ��������

	CreateProcess(TEXT(ParentPath),									    //�������� �������� 
		NULL,														    //�������� ��������� ������
		NULL,                                                           //�������� ������ ��������
		NULL,                                                           //�������� ������ ������ 
		FALSE,                                                          //���� ������������ �������� ��������
		CREATE_NEW_CONSOLE,                                             //����� ������� �������� ����� ������� ������ ����, ����� ������������ ������������.
		NULL,                                                           //��������� �� ���� �����
		NULL,                                                           //������� ���� ��� �������
		&startupInfoParent,                                             //��������� �� ��������� STARTUPINFO
		&processInfoParent);                                            //��������� �� ��������� PROCESS_INFO

	Sleep(1000);                                                         //�������� ������� ��������

	deque<HANDLE> deque_of_events, deque_of_processes;                  //������������ ������� ��� �������� ��������� � �������

	HANDLE plus_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event_+");  //������� ���������� ���������� ������� ��� ������ '+'
	HANDLE minus_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event_-");
	HANDLE quit_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event_q");

	const HANDLE events_mas[3] =                                        //������ ������� ��� �������������� ��������
	{
		plus_event,
		minus_event,
		quit_event
	};

	STARTUPINFO startupInfoChild;                                               //��������� STURTUPINFO ��� ��������� ��������
	ZeroMemory(&startupInfoChild, sizeof(STARTUPINFO));                         //��������� ��������� ��������� STURTUPINFO
	PROCESS_INFORMATION processInfoChild;                                       //��������� PROCESS_INFORMATION ��� ��������� ��������

	while (true)
	{
		cout << "Number of processes: " << deque_of_processes.size() << endl;
		switch (WaitForMultipleObjects(3,                                      //���������� ������������ �������� � �������
			events_mas,                                                    //������ ������������ �������
			FALSE,                                                         //������� ������������, ����� ��������� ������ �� �������� ����������� ��� ����������
			100))                                                          //�������� ������� �������� � �������������                    
		{
		case WAIT_FAILED:                                                      //������� �� ���������
			cout << "Code error: " << GetLastError() << endl;
			system("pause");
			break;

		case WAIT_TIMEOUT:                                                     //����� �������� ��������, � �������, �������� ���������� bWaitAll , �� �����������
			for (int i = 0; i < deque_of_events.size(); i++)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(wait_event, 1)) {
					ResetEvent(wait_event);                                    //����� ��������� �������
					break;
				}
				Sleep(1);
				PulseEvent(deque_of_events[i]);                                //������������ ������ �������� �������
				WaitForSingleObject(deque_of_events[i], INFINITE);             //����������� �������� ������ �� �������
			}
			break;

		case WAIT_OBJECT_0 + 0:                                                      //���� ����������� ������� '+'
			if (!CreateProcess(TEXT(ChildPath),
				const_cast<LPSTR>(("Line - " + to_string(process_counter)).c_str()), //�������� ������ � ��������� ������ ��� �� ������������ ������
				NULL,
				NULL,
				FALSE,
				NULL,
				NULL,
				NULL,
				&startupInfoChild,
				&processInfoChild))
				cout << "Error while creating process" << endl;
			else
			{
				deque_of_processes.push_back(processInfoChild.hProcess);
				deque_of_events.push_back(CreateEvent(NULL, true, false, ("Event_" + to_string(processInfoChild.dwProcessId)).c_str()));
				process_counter++;
				ResetEvent(events_mas[0]);
			}
			break;

		case WAIT_OBJECT_0 + 1:                                                        //���� ����������� ������� '-'
			if (!deque_of_processes.empty())
			{
				process_counter--;
				TerminateProcess(deque_of_processes.back(), 0);
				CloseHandle(deque_of_events.back());
				CloseHandle(deque_of_processes.back());
				deque_of_events.pop_back();
				deque_of_processes.pop_back();
				ResetEvent(events_mas[1]);
			}
			break;

		case WAIT_OBJECT_0 + 2:                                                         //���� ����������� ������� 'q'
			while (!deque_of_processes.empty())
			{
				TerminateProcess(deque_of_processes.back(), 0);
				CloseHandle(deque_of_events.back());
				CloseHandle(deque_of_processes.back());
				deque_of_events.pop_back();
				deque_of_processes.pop_back();
				ResetEvent(events_mas[2]);
			}
			return 1;
		}
		system("cls");
	}
	return 0;
}