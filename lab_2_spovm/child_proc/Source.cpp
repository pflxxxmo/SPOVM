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

	HANDLE plus_event = CreateEvent(NULL, true, FALSE, "Event_+");                                            //Создание события для добавления процессов 
	HANDLE minus_event = CreateEvent(NULL, true, FALSE, "Event_-");
	HANDLE quit_event = CreateEvent(NULL,                                                                     //Атрибут защиты события  Если этот параметр равен NULL , дескриптор не может наследоваться дочерними процессами.
		true,                                                                     //Параметр атвтоматического (FALSE) и мануального сброса (TRUE)
		FALSE,                                                                    //Сигнализатор начала процесса
		"Event_q");                                                               //Имя объекта события

	HANDLE wait_event = OpenEvent(EVENT_ALL_ACCESS,                                                           //Доступ к объекту события
		FALSE,                                                                      //Параметр начледования дескриптора
		"Wait_event");                                                              //Имя объекта события

	while (true)
	{
		switch (_getch())
		{
		case '+':
			SetEvent(plus_event);                                                                             //Установка события
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