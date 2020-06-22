#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

int main(int argc, char const *argv[])
{
	int a = 42;
	printf("Press SPACE to leave\n");
	printf("Press ENTER to update\n");
	while (1)
	{
		printf("\rA: %d | V: %d", &a, a);
		if(getch() == ' ')
		{
			break;
		}
	}
	
	return 0;
}