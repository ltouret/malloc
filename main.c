#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <string.h>

//135168 first malloc

int main()
{
	int i = 100000;
	while (i--)
	{
		char *ptr = mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		ptr[0] = 'a';
		//double res = (unsigned long) ptr / 4096.0;
		//printf("%p %lu %f\n", ptr, (unsigned long) ptr, res);
	}
	//munmap(ptr, 1);
	//while (1);
	return 0;
}
