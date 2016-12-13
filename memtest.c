#include <stdio.h>
#include <stdlib.h>

void main()
{
	int *a = (int*)malloc(1024 * 1024 * 1024 * sizeof(int));
	a[1024*1024*1024 - 1] = 9;
	printf("%d", a[1024 * 1024 * 1024 - 1]);
}
