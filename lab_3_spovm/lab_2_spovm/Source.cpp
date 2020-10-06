#include "Header.h"
#define BUFFER_SIZE 512

void Connect(struct Data* data)
{
	data->semaphore = OpenSemaphore(					// Записываем handle семафора в структуру 
		SEMAPHORE_ALL_ACCESS,                           // Cпецифицирует все возможные флаги доступа к семафору
		FALSE,
		TEXT("Semaphore"));                             // Имя семафора

	data->channel = CreateFile(
		TEXT("\\\\.\\pipe\\$MyPipe$"),                      // имя файла
		GENERIC_READ | GENERIC_WRITE,                       // режим доступа
		NULL,                                               // совместный доступ
		NULL,                                               // SD (дескр. защиты)
		OPEN_EXISTING,                                      // как действовать (открыть существующий канал)
		NULL,                                               // атрибуты файла
		NULL);                                              // дескр.шаблона файла

	if (ConnectNamedPipe(
		data->channel,                                          // идентификатор именованного канала
		NULL))                                                  // адрес структуры OVERLAPPED
	{
		printf("No connection!\n");
		return;
	}
	else
		printf("Server connected... \n");
	ReleaseSemaphore(                                                // освободить счетчик
		data->semaphore,                                             // handle семафора
		1,                                                           // на сколько изменять счетчик
		NULL);                                                       // предыдущее значение
}

void Run(struct Data* data)
{
	char buffer[BUFFER_SIZE];
	int k;
	while (true)
	{
		DWORD dwRead;
		if (ReadFile(data->channel, buffer, BUFFER_SIZE, &dwRead, NULL))
			printf("%s\n", buffer);

		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

		if (ReadFile(
			data->channel,                                               // дескриптор файла
			buffer,                                                      // буфер данных
			BUFFER_SIZE,                                                 // число байтов для чтения
			&dwRead,                                                     // число прочитанных байтов
			NULL))                                                       // асинхронный буфер
		{
			if (strcmp(buffer, "exit\0"))
			{
				printf("Server message: ");
				puts(buffer);
			}
			else
				Close(data);
		}
		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

		DWORD dwWritten;
		if (!WriteFile(
			data->channel,                                              // дескриптор файла  
			"Client ready",                                             // буфер данных
			BUFFER_SIZE,                                                // число байтов для записи
			&dwWritten,                                                 // число записанных байтов
			NULL))                                                      // асинхронный буфер
			break;

		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

		printf("Enter your message: ");
		fflush(stdin);
		fgets(buffer, 100, stdin);
		k = strlen(buffer);
		buffer[k - 1] = '\0';
		DWORD  dwWritten1;
		if (!WriteFile(data->channel, buffer, BUFFER_SIZE, &dwWritten1, NULL))
			break;

		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

	}
}

void Close(struct Data* data)
{
	CloseHandle(data->channel);														//Закрывает дескриптор открытого объекта.
	CloseHandle(data->semaphore);
}

int main()
{
	MoveWindow(GetConsoleWindow(), 900, 100, 800, 700, true);
	struct Data data;

	Connect(&data);
	Run(&data);
	Close(&data);
	return 0;
}