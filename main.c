//! move this to main test
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "malloc.h"

#define M 1024 * 1024

void	print(char *s)
{
	write(1, s, strlen(s));
}

int		main(void)
{
	// char	*addr;
	// char *addr1;
	// char *addr2;
	// char *addr3;
	// int		i;

	// i = 0;
	// while (i < 1024)
	// {
	// 	addr = (char*)malloc(1024);
	// 	addr[0] = 42;
	// 	free(addr);
	// 	i++;
	// }



	// addr1 = malloc(16 * M);
	// strcpy(addr1, "Bonjours\n");
	// print(addr1);
	// addr2 = malloc(16 * M);
	// addr3 = realloc(addr1, 128 * M);
	// addr3[127 * M] = 42;
	// print(addr3);

	// addr = malloc(16);
	// free(NULL);
	// free((void *)addr + 5);
	// if (realloc((void *)addr + 5, 10) == NULL)
	// 	print("Bonjours\n");


	// malloc(1024);
	// malloc(1024 * 32);
	// malloc(1024 * 1024);
	// malloc(1024 * 1024 * 16);
	// malloc(1024 * 1024 * 128);

	char *addr[180];

  addr[0] = malloc(20);
  addr[1] = malloc(40);
  addr[2] = malloc(4);
  addr[3] = malloc(15);
  addr[4] = malloc(42);
  addr[5] = malloc(89);
  addr[6] = malloc(589);
  addr[7] = malloc(400);
  addr[8] = malloc(123);
  show_alloc_mem();

  free(addr[0]);
  free(addr[3]);
  free(addr[5]);
  free(addr[7]);
  show_alloc_mem();

  addr[0] = malloc(42);
  addr[3] = malloc(2000);
  addr[5] = malloc(10000);
  printf("------------- s_a_m -------------\n");
  show_alloc_mem();

  strncpy(addr[1], "Ceci est un test", 30);
  printf("le contenu de la zone de 40 octets: |%s|\n", addr[1]);

  addr[9] = realloc(addr[1], 156);
  printf("------------- s_a_m -------------\n");
  show_alloc_mem();

  printf("le contenu de la zone de 40 octets devenu 156: |%s|\n", addr[9]);
  printf("le contenu de l'ancienne zone mémoire de la zone de 40 octets"\
      " devenu 156: |%s|\n",
      addr[1]);
	show_alloc_mem();


//   int i = 0;
//   for (i = 0; i < 180; i++)
//     addr[i] = malloc(42);
//   for (i = 0; i < 180; i++)
//     addr[i] = malloc(642 + (i % 4));
//   for (i = 0; i < 180; i++)
//     addr[i] = malloc(14292 + (i % 400));
//   for (i = 0; i < 180; i++)
//     free(addr[i]);
//   addr[0] = malloc(420133);
//   for (i = 0; i < 80; i++)
//     addr[i] = malloc(3);
//   show_alloc_mem();


	return (0);
}