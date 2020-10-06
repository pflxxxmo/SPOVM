#include <stdio.h>
#include <windows.h>
#include <ctime>



int main()
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcInfo;
	TCHAR TXT[] = TEXT("D:\\Visual Studio\\4\\lab 1\\Debug\\Child.exe");

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ProcInfo, sizeof(ProcInfo));

	printf("Parent process was run.\n");

	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	printf("Current Datetime:%s", asctime(timeinfo));

	if (!CreateProcess(TXT,  
		NULL,          
		NULL,                  
		NULL,                  
		FALSE,                
		CREATE_NEW_CONSOLE,                    
		NULL,                 
		NULL,                 
		&StartupInfo,         
		&ProcInfo)           
		)

		printf("CreateProcess failed.\n");

	
	WaitForSingleObject(ProcInfo.hProcess, INFINITE);

	printf("End of child process.\n");

	 
	CloseHandle(ProcInfo.hProcess);
	CloseHandle(ProcInfo.hThread);

}