#include <iostream>
#include <Windows.h>
#include <experimental/filesystem>
using namespace std;
namespace fs = std::experimental::filesystem;

struct threadMessage																					// Структура потоковой синхронизации 
{
	threadMessage(HANDLE* eventHandle_, HANDLE* ioHandle_)
	{
		eventHandle = eventHandle_;
		ioHandle = ioHandle_;
	}
	HANDLE* eventHandle;
	HANDLE* ioHandle;
};

typedef DWORD(WINAPI* importFunc)(void* par);
importFunc ReaderThread;
importFunc WriterThread;

int main()
{
	HINSTANCE dllHandler = LoadLibrary("ReadWriteDLL");													//Загрузка динамической библиотеки
	if (!dllHandler)
		cout << "END OF PROGRAMM!\n";

	if (((ReaderThread = (importFunc)GetProcAddress(dllHandler, "_ReaderThread@4")) == NULL) ||			// Получения адреса функции-обработчика
		(WriterThread = (importFunc)GetProcAddress(dllHandler, "_WriterThread@4")) == NULL)
		cout << "ERROR IMPORTING " << GetLastError() << "\n";

	HANDLE readPipeHandle, writePipeHandle, syncEventHandle = CreateEventA(NULL, TRUE, FALSE, NULL);    // События для синхронизации между чтением и записью

	if (CreatePipe(&readPipeHandle, &writePipeHandle, NULL, 32 * BUFSIZ) == 0)							// Создание канала с передачей дескриптора чтения и записи
																										// и передачей размера передаваемого под канал
		cout << "Error creating pipe" << endl;

	HANDLE WriterThreadHandle = CreateThread(NULL, NULL, WriterThread, new threadMessage(&syncEventHandle, &readPipeHandle), NULL, NULL);
	//Инициализация события с передачей дескриптора создаваемого потока. Передача потоковой функции записи и параметра синхронизации
	HANDLE ReaderThreadHandle = CreateThread(NULL, NULL, ReaderThread, new threadMessage(&syncEventHandle, &writePipeHandle), NULL, NULL);
	//Инициализация события с передачей дескриптора создаваемого потока. Передача потоковой функции чтения и параметра синхронизации
	WaitForSingleObject(ReaderThreadHandle, INFINITE);				// Ожидание завершения чтения
	Sleep(300);

	FreeLibrary(dllHandler);										// Выгрузка библиотеки
	getchar();
	return 0;
}