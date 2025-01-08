#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "malloc.h"

#define M 1024 * 1024

int		main(void)
{
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

    char *bad_realloc;

    bad_realloc = malloc(16);
    free(NULL);
    free((void *)bad_realloc + 5);
    if (realloc((void *)bad_realloc + 5, 10) == NULL) {
        printf("Bonjour\n");
    }
    // free(bad_realloc);
	show_alloc_mem();
	return (0);
}