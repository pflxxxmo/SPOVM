#include "Header.h"
#define BUFFER_SIZE 512

void Connect(struct Data* data)
{
	data->semaphore = OpenSemaphore(					// ���������� handle �������� � ��������� 
		SEMAPHORE_ALL_ACCESS,                           // C������������ ��� ��������� ����� ������� � ��������
		FALSE,
		TEXT("Semaphore"));                             // ��� ��������

	data->channel = CreateFile(
		TEXT("\\\\.\\pipe\\$MyPipe$"),                      // ��� �����
		GENERIC_READ | GENERIC_WRITE,                       // ����� �������
		NULL,                                               // ���������� ������
		NULL,                                               // SD (�����. ������)
		OPEN_EXISTING,                                      // ��� ����������� (������� ������������ �����)
		NULL,                                               // �������� �����
		NULL);                                              // �����.������� �����

	if (ConnectNamedPipe(
		data->channel,                                          // ������������� ������������ ������
		NULL))                                                  // ����� ��������� OVERLAPPED
	{
		printf("No connection!\n");
		return;
	}
	else
		printf("Server connected... \n");
	ReleaseSemaphore(                                                // ���������� �������
		data->semaphore,                                             // handle ��������
		1,                                                           // �� ������� �������� �������
		NULL);                                                       // ���������� ��������
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
			data->channel,                                               // ���������� �����
			buffer,                                                      // ����� ������
			BUFFER_SIZE,                                                 // ����� ������ ��� ������
			&dwRead,                                                     // ����� ����������� ������
			NULL))                                                       // ����������� �����
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
			data->channel,                                              // ���������� �����  
			"Client ready",                                             // ����� ������
			BUFFER_SIZE,                                                // ����� ������ ��� ������
			&dwWritten,                                                 // ����� ���������� ������
			NULL))                                                      // ����������� �����
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
	CloseHandle(data->channel);														//��������� ���������� ��������� �������.
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