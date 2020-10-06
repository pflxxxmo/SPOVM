#include <windows.h>
#include <stdio.h>
#include <ctime>



int main()
{
	printf("Child process was run.\n");
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	printf("Current Datetime:%s", asctime(timeinfo));
	system("pause");
	return 0;
}