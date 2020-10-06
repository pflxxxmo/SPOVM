#include "Header.h"

using namespace std;
namespace fs = std::experimental::filesystem;						// Простанство имен для доступа к функциям работы с файлами

struct threadMessage														// Структура потоковой синхронизации 
{
	threadMessage(HANDLE* eventHandle_, HANDLE* ioHandle_)
	{
		eventHandle = eventHandle_;
		ioHandle = ioHandle_;
	}
	HANDLE* eventHandle;
	HANDLE* ioHandle;
};

extern "C" __declspec(dllexport) DWORD WINAPI ReaderThread(void* par)					// Экспортирование из dll с компановкой на С, инициализация потоковой функции
{
	threadMessage message = *(threadMessage*)par;
	fs::path filesPath(fs::current_path() /= fs::path("files"));						// Получение папки с файлами
	fs::create_directories(filesPath);
	HANDLE eventHandle = CreateEvent(NULL, FALSE, FALSE, "Lycik");						//
	if (eventHandle == NULL)
		cout << "Error creating event" << endl;

	for (auto it : fs::directory_iterator(filesPath))									// Передача итератора на начало директории файлов
	{
		cout << "Reading " << it.path().filename() << endl;								// Указывает на название читаемого файла
		char inputBuffer[BUFSIZ + 1];													// Буфер считывания текста с файла
		string readedStr = "";
		int readedBytesCount = 0;														// Размер считаемого файла

		HANDLE fileHandle = CreateFileW(it.path().c_str(),								// Открытие файла
			GENERIC_READ,																// Запрос на чтение
			NULL,																		// Режим доступа
			NULL,																		// Аттрибут безопаности
			OPEN_EXISTING,																// Открытие файла (Если он существует)
			FILE_FLAG_OVERLAPPED,														// Открытие для ассинхронного чтения
			NULL);																		// Допутимый дескриптор 
		if (fileHandle == INVALID_HANDLE_VALUE)
			cout << "Error opening file" << endl;

		OVERLAPPED params;																// Структура асинхронного ввода/вывода
		params.hEvent = eventHandle;													// Передача синхронизируемого события
		params.Offset = 0;																// Младшее слово ( с которого производится операция)
		params.OffsetHigh = 0;															// Старшее слово смещения

		do
		{
			ReadFile(fileHandle, inputBuffer, BUFSIZ, (LPDWORD)&readedBytesCount, &params); // Чтение файла с передачей переменной структуры Асинхронного в/в
			WaitForSingleObject(eventHandle, INFINITE);
			inputBuffer[params.InternalHigh] = 0;										// Получение кол-ва переданных байт
			readedStr += inputBuffer;
			params.Offset += params.InternalHigh;										// Указание на смещение с младшего слова
		} while (params.InternalHigh == BUFSIZ);										// Пока не конец файла
		CloseHandle(fileHandle);

		if (readedStr.size())															// Если полученная строка не нулевая
		{
			WriteFile(*message.ioHandle, readedStr.c_str(), readedStr.size(), NULL, NULL);
			Sleep(100);
			PulseEvent(*message.eventHandle);
			WaitForSingleObject(*message.eventHandle, INFINITE);
		}
	}
	CloseHandle(eventHandle);
	return 0;
}

extern "C" __declspec(dllexport) DWORD WINAPI WriterThread(void* par)					// Экспортирование из dll с компановкой на С, инициализация потоковой функции
{
	threadMessage message = *(threadMessage*)par;
	HANDLE fileHandle = CreateFileA("output.txt", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL); // Создание файла для записи
	HANDLE eventHandle = CreateEvent(NULL, FALSE, FALSE, "Lycik");

	OVERLAPPED params;																// Структура асинхронного ввода/вывода
	params.hEvent = eventHandle;													// Передача синхронизируемого события
	params.Offset = 0;																// Младшее слово ( с которого производится операция)
	params.OffsetHigh = 0;															// Старшее слово смещения

	while (true)
	{
		WaitForSingleObject(*message.eventHandle, INFINITE);
		string readedString;
		while (true)
		{
			char buffer[BUFSIZ + 1];
			int bytesReaded;
			ReadFile(*message.ioHandle, buffer, BUFSIZ, (LPDWORD)&bytesReaded, NULL); // Чтение с канала в строку
			buffer[bytesReaded] = 0;
			readedString += buffer;
			if (string(buffer).size() != BUFSIZ)
				break;

		}

		WriteFile(fileHandle, readedString.c_str(), readedString.size(), NULL, &params);//Запись полученной строки в файл вывода через дескриптор
		params.Offset += readedString.size();
		WaitForSingleObject(eventHandle, INFINITE);
		Sleep(100);
		PulseEvent(*message.eventHandle);
	}
	CloseHandle(fileHandle);
	return 0;
}