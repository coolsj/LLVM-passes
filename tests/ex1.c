#include "stdio.h"

int f(int a, int b)
{
	int result = 0;
	int c = a+b;
	result += c*5;
	int d = a+b;
	result += d*100;
	printf("result: %d\n", result);
	return result;
}
