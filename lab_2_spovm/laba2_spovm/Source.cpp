#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>
#include <deque>

constexpr auto ParentPath = "D:\\Visual Studio\\4\\laba2_spovm\\Debug\\child_proc.exe"; //Адрес родительского процесса
constexpr auto ChildPath = "D:\\Visual Studio\\4\\laba2_spovm\\Debug\\output.exe"; //Адрес дочернего процесса


using namespace std;

int main()
{
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);						//Установка окна в полноэкранный режим
	int process_counter = 1;										    //Счетчик процессов для последующей передачи его в номер строки
	HANDLE wait_event = CreateEvent(NULL, true, FALSE, "Wait_event");   //Создание события для промежуточного ожидания события

	STARTUPINFO startupInfoParent;										//Структура STARTUPINFO для родительского процесса
	ZeroMemory(&startupInfoParent, sizeof(STARTUPINFO));				//Обнуление указателя структуры STARTUPINFO
	PROCESS_INFORMATION processInfoParent;								//Структура PROCESS_INFORMATION для родительского процесса

	CreateProcess(TEXT(ParentPath),									    //Создание процесса 
		NULL,														    //Атрибуты командной строки
		NULL,                                                           //Атрибуты защиты процесса
		NULL,                                                           //Атрибуты защиты потока 
		FALSE,                                                          //Флаг наследования текущего процесса
		CREATE_NEW_CONSOLE,                                             //Новый процесс получает новую консоль вместо того, чтобы унаследовать родительскую.
		NULL,                                                           //Указатель на блок среды
		NULL,                                                           //Текущий диск или каталог
		&startupInfoParent,                                             //Указатель на структуру STARTUPINFO
		&processInfoParent);                                            //Указатель на структуру PROCESS_INFO

	Sleep(1000);                                                         //Ожидание запуска процесса

	deque<HANDLE> deque_of_events, deque_of_processes;                  //Двусторонняя очередь для хранения процессов и событий

	HANDLE plus_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event_+");  //Открыть дескриптор созданного события для кнопки '+'
	HANDLE minus_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event_-");
	HANDLE quit_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event_q");

	const HANDLE events_mas[3] =                                        //Массив событий для множественного ожидания
	{
		plus_event,
		minus_event,
		quit_event
	};

	STARTUPINFO startupInfoChild;                                               //Структура STURTUPINFO для дочернего процесса
	ZeroMemory(&startupInfoChild, sizeof(STARTUPINFO));                         //Обнуление указателя структуры STURTUPINFO
	PROCESS_INFORMATION processInfoChild;                                       //Структура PROCESS_INFORMATION для дочернего процесса

	while (true)
	{
		cout << "Number of processes: " << deque_of_processes.size() << endl;
		switch (WaitForMultipleObjects(3,                                      //Количество дескрипторов объектов в массиве
			events_mas,                                                    //Массив дескрипторов объекта
			FALSE,                                                         //Функция возвращается, когда состояние любого из объектов установлено как сигнальное
			100))                                                          //Интервал времени ожидания в миллисекундах                    
		{
		case WAIT_FAILED:                                                      //Функция не выполнена
			cout << "Code error: " << GetLastError() << endl;
			system("pause");
			break;

		case WAIT_TIMEOUT:                                                     //Истек интервал ожидания, и условия, заданные параметром bWaitAll , не выполняются
			for (int i = 0; i < deque_of_events.size(); i++)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(wait_event, 1)) {
					ResetEvent(wait_event);                                    //Сброс состояния события
					break;
				}
				Sleep(1);
				PulseEvent(deque_of_events[i]);                                //Освобождение потока ожидания события
				WaitForSingleObject(deque_of_events[i], INFINITE);             //Бесконечное ожидание одного из событий
			}
			break;

		case WAIT_OBJECT_0 + 0:                                                      //Если выполнилось событие '+'
			if (!CreateProcess(TEXT(ChildPath),
				const_cast<LPSTR>(("Line - " + to_string(process_counter)).c_str()), //Передача строки в командную строку для ее последующего вывода
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

		case WAIT_OBJECT_0 + 1:                                                        //Если выполнилось событие '-'
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

		case WAIT_OBJECT_0 + 2:                                                         //Если выполнилось событие 'q'
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