#include "stdio.h"

int func(int x, int y)
{
	int a, b, c, d;
	a = x+y;
	b = x*y;
	c = x-y;
	d = x/y;
	for(int i=0; i<10; i++)
	{
		d += x+y;
		int temp=x+y;
		if(temp)
		{
			b += x-y;
			c += x*y;
			d += x-y;
			a += x*y;
		}
		else
		{
			b += x/y;
			c += x+y;
			d += x/y;
			a += x+y;
		}
	}
	return a+b+c+d;
}

int main()
{
	printf("result: %d\n", func(10, 20));
	return 0;
}
