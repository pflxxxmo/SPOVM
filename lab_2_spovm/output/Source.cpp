#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

int main()
{
	HANDLE child_process_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, ("Event_" + to_string(GetCurrentProcessId())).c_str()); //Открытие события дочернего процесса
	string process_number = to_string(GetCurrentProcessId()), message = GetCommandLineA();                                  //Получение значений переданных в командной строке
	while (true)
	{
		WaitForSingleObject(child_process_event, INFINITE);                                                                 //Бесконечное ожидание дочернего процесса
		for (char a : message + "   id " + process_number)                                                                  //Формирование символов из строки
		{
			cout << a;
			Sleep(20);
		}
		PulseEvent(child_process_event);                                                                                    //Освобождает один поток который ждал событие, после чего возвращает событие в сброшенное (unset) состояние (спусковой крючок)
		cout << endl;
	}
	return 0;
}