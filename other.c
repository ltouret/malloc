#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUCKET_SIZE sizeof(t_bucket)
#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE getpagesize()

//#define TINY ((PAGE * 7) - ZONE_SIZE) / (BLOCK_SIZE )
#define TINY_SIZE 240
#define SMALL_SIZE 1400
#define LARGE_SIZE 5400
#define ALLOCATIONS 105
//#define TINY (((((ALLOCATIONS * BLOCK_SIZE) + (ALLOCATIONS * 280)) + ZONE_SIZE) / PAGE) + 1)
#define TINY (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE) + 1)
//#define TINY (((((AA * BLOCK_SIZE) + ZONE_SIZE + (AA * 240)) / PAGE) + 1))
//#define BB (((PAGE * TINY) - ZONE_SIZE) / AA) - (BLOCK_SIZE)
//#define BB (((PAGE * 28) - ZONE_SIZE) / 105) - (BLOCK_SIZE)
#define TINY (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE) + 1)
#define SMALL (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE) + 1)

typedef struct s_bucket {
    struct t_zone *tiny;
    struct t_zone *small;
    struct t_zone *large;
} t_bucket;

typedef struct s_block {
    struct t_block *next;
    struct t_block *prev;
    size_t size;
    size_t free; //padding
} t_block;

typedef struct s_zone {
    struct t_zone *next;
    t_block *block;
    size_t free_space;
    size_t type; //padding
} t_zone;

t_bucket allocs;

size_t  go_next_block(size_t size)
{
    return (size + 15) & ~ 15;
}

void    *create_tiny(size_t size)
{
    void *ret = NULL;
    if (allocs.tiny == NULL)
    {
        ret = mmap ( NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
    }
    return ret;
}

void    *my_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    if (size < 10000) 
        create_tiny(5); // check if tiny doesnt exist if it does check tehres space if there is use this
    return NULL;
}

// add better format here
void    hex_dump(void *s, size_t n)
{
    size_t i = 0;
    printf("Hex dump:\n");
    while (i < n){
        printf("%lu %x\n", i, ((char *)s)[i]);
        i++;
    }
    // printf("\n");
}

void    ft_bzero(void *s, size_t n)
{
    while (n--){
        // ((char *)s)[n] = 125;
        // printf("%x", ((char *)s)[n]);
        ((char *)s)[n] = '\0';
    }
}

int main()
{
    // int m = (10 + 15) & ~ 15;
	printf("%ld %ld %ld %d\n", SMALL, TINY, BLOCK_SIZE, getpagesize());
    printf("%lu \n", go_next_block(0));
    char *five = my_malloc(5);
    // hex_dump(&allocs, sizeof(t_bucket));
    ft_bzero(&allocs, sizeof(t_bucket));
    printf("%lu %lu %lu\n", sizeof(t_bucket), sizeof(t_zone), sizeof(t_block));
    
}
