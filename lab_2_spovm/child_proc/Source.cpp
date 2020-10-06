#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>

using namespace std;

int main()
{
	cout << "\t\t<***********************************>" << endl;
	cout << "\t\t\tCreate process: +" << endl;
	cout << "\t\t\tKill last process: -" << endl;
	cout << "\t\t\tExit : q" << endl;
	cout << "\t\t<***********************************>" << endl;

	HANDLE plus_event = CreateEvent(NULL, true, FALSE, "Event_+");                                            //�������� ������� ��� ���������� ��������� 
	HANDLE minus_event = CreateEvent(NULL, true, FALSE, "Event_-");
	HANDLE quit_event = CreateEvent(NULL,                                                                     //������� ������ �������  ���� ���� �������� ����� NULL , ���������� �� ����� ������������� ��������� ����������.
		true,                                                                     //�������� ���������������� (FALSE) � ����������� ������ (TRUE)
		FALSE,                                                                    //������������ ������ ��������
		"Event_q");                                                               //��� ������� �������

	HANDLE wait_event = OpenEvent(EVENT_ALL_ACCESS,                                                           //������ � ������� �������
		FALSE,                                                                      //�������� ������������ �����������
		"Wait_event");                                                              //��� ������� �������

	while (true)
	{
		switch (_getch())
		{
		case '+':
			SetEvent(plus_event);                                                                             //��������� �������
			SetEvent(wait_event);
			break;
		case '-':
			SetEvent(minus_event);
			SetEvent(wait_event);
			break;
		case 'q':
			SetEvent(quit_event);
			SetEvent(wait_event);
			return 1;
		}
	}
	return 0;
}