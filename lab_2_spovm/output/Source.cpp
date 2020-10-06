#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

int main()
{
	HANDLE child_process_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, ("Event_" + to_string(GetCurrentProcessId())).c_str()); //�������� ������� ��������� ��������
	string process_number = to_string(GetCurrentProcessId()), message = GetCommandLineA();                                  //��������� �������� ���������� � ��������� ������
	while (true)
	{
		WaitForSingleObject(child_process_event, INFINITE);                                                                 //����������� �������� ��������� ��������
		for (char a : message + "   id " + process_number)                                                                  //������������ �������� �� ������
		{
			cout << a;
			Sleep(20);
		}
		PulseEvent(child_process_event);                                                                                    //����������� ���� ����� ������� ���� �������, ����� ���� ���������� ������� � ���������� (unset) ��������� (��������� ������)
		cout << endl;
	}
	return 0;
}