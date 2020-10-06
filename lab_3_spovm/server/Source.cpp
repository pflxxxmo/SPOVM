#include "Header.h"
#define BUFFER_SIZE 512
#define CLIENT_LOC "D:\\Visual Studio\\4\\lab_3_spovm\\Debug\\lab_2_spovm.exe"

void open_client_window()
{
	STARTUPINFO StartUpInfo;                                        // �������� ���� ������ ��������
	PROCESS_INFORMATION ProcessInformation;                         // ��������� �� ��������� PROCESS_INFORMATION
	ZeroMemory(&StartUpInfo, sizeof(StartUpInfo));                  // ��������� StartupInfo
	StartUpInfo.cb = sizeof(StartUpInfo);                           // ��������� ������� ��������� STARTUPINFO � ������ 
	ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));
	if (!CreateProcess(TEXT(CLIENT_LOC),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&StartUpInfo,
		&ProcessInformation))
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		system("pause >> NUL");
		exit(-1);
	}
}

void Connect(struct Data* data)
{
	data->semaphore = CreateSemaphore(
		NULL,                                                  // ������� �������
		0,                                                     // ������������������ ��������� ��������� ��������
		1,                                                     // ������������ ���������� ���������
		L"Semaphore1");                                        // ��� �������

	data->channel = CreateNamedPipe(
		TEXT("\\\\.\\pipe\\$MyPipe$"),                         // ��������� �� ��� ������
		PIPE_ACCESS_DUPLEX,                                    // ����� ���������������; ��� � ��������� � ���������� �������� � ����� ������ � ������ � ���������� � ����.
		PIPE_TYPE_MESSAGE                                      // ������ ������������ � ����� ��� ����� ���������
		| PIPE_READMODE_MESSAGE                                // ������ ����������� � ������ ��� ����� ���������. 
		| PIPE_WAIT,                                           // ����������� ������� � ���� ��������.
		PIPE_UNLIMITED_INSTANCES,                              // MAX ����� �������� ����� ���� ������������ ������ �������� ��������� ��������
		100,                                                   // ��������� ������ ������, � ������
		100,                                                   // �������� ������ ������, � ������
		NMPWAIT_USE_DEFAULT_WAIT,                              // time-out �����, � �������������
		NULL);                                                 // ��������� �� �������� ������������

	printf("Waiting... \n");

	open_client_window();

	if (!ConnectNamedPipe(data->channel, NULL))
	{
		printf("No connection! \n");
		return;
	}
	else
		printf("Client connected... \n");
	ReleaseSemaphore(data->semaphore, 1, NULL);
}

void Run(struct Data* data)
{
	char buffer[BUFFER_SIZE];
	int k;
	while (true)
	{
		DWORD dwWritten;
		if (!WriteFile(data->channel, "Server ready", BUFFER_SIZE,
			&dwWritten, NULL))
			break;

		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

		printf("Enter your message: ");
		fflush(stdin);
		fgets(buffer, BUFFER_SIZE, stdin);
		k = strlen(buffer);
		buffer[k - 1] = '\0';
		DWORD  dwWritten1;
		if (!WriteFile(data->channel, buffer, BUFFER_SIZE,
			&dwWritten1, NULL))
			break;
		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

		DWORD dwRead;
		if (ReadFile(data->channel, buffer, BUFFER_SIZE,
			&dwRead, NULL))
			printf("%s\n", buffer);
		ReleaseSemaphore(data->semaphore, 1, NULL);

		WaitForSingleObject(data->semaphore, INFINITE);

		if (ReadFile(data->channel, buffer, BUFFER_SIZE, &dwRead, NULL))
		{
			if (strcmp(buffer, "exit\0"))
			{
				printf("Client message: ");
				puts(buffer);
			}
			else
				Close(data);
		}
		ReleaseSemaphore(data->semaphore, 1, NULL);
	}
}

void Close(struct Data* data)
{
	CloseHandle(data->channel);
	CloseHandle(data->semaphore);
}

int main()
{
	MoveWindow(GetConsoleWindow(), 100, 100, 800, 700, true);
	struct Data data;
	printf("Server window\t\tPrint ''exit'' to quit from program\n");
	Connect(&data);
	Run(&data);

	Close(&data);
	return 0;
}