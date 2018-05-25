#include "stdio.h"

int func(int a, int b)
{
	int c=0, d=0;
	int temp = a+b;
	if(a>b)
		c = a+b-1;
	else
		d = a+b+1;
	return (a+b+c+d);
}

int main()
{
	printf("result: %d\n", func(10,20));
	return 0;
}
