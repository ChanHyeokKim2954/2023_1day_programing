#include <stdio.h>

int main()
{
	char* s1 = "hello";
	s1[0] = 'A';

	printf("%c\n", s1[0]);
	return 0;
}