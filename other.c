#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUCKET_SIZE sizeof(t_bucket)
#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE getpagesize()

#define TINY_SIZE 240
#define SMALL_SIZE 1400
#define LARGE_SIZE 5400
#define ALLOCATIONS 105
#define TINY_ZONE (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE
#define SMALL_ZONE (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE

// ADD tree afer each malloc with size and pointer to go through each time and do the full block in half the time

typedef struct s_block {
	struct s_block *next;
	struct s_block *prev;
	size_t size;
	size_t free; //padding
} t_block;

typedef struct s_zone {
	struct s_zone *next;
	t_block *block;
	size_t free_space;
	size_t type; //padding
} t_zone;

typedef struct s_bucket {
	t_zone *tiny;
	t_zone *small;
	t_zone *large;
} t_bucket;

t_bucket	allocs;

void	ft_bzero(void *s, size_t n)
{
	while (n--)
	{
		// ((char *)s)[n] = 125;
		// printf("%x", ((char *)s)[n]);
		((char *)s)[n] = '\0';
	}
}

size_t	go_next_block(size_t size)
{
	return (size + 15) & ~ 15;
}

void	*create_tiny(size_t size)
{
	void	*ret = NULL;
	//t_zone	*
	if (allocs.tiny == NULL)
	{
		//printf("tiny %lu tiny_block\n", TINY_ZONE );
		allocs.tiny = mmap ( NULL, TINY_ZONE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
		
		// init and populate the zone in another func?
		ft_bzero(allocs.tiny, TINY_ZONE); // ft_bzero needed?
		allocs.tiny->next = NULL; // already done by ft_bzero
		//printf("1 allocs.tiny %lu allocs.tiny->next %lu allocs.tiny->block %lu\n", allocs.tiny, &allocs.tiny->type, &allocs.tiny->block);
		allocs.tiny->block = (void *)allocs.tiny + ZONE_SIZE; 
		allocs.tiny->free_space = TINY_ZONE - ZONE_SIZE;
		printf("2 allocs.tiny %lu allocs.tiny->block %lu\n", allocs.tiny, allocs.tiny->block);
	}
	// else if allocs.tiny isnt null and has free_space
	else if (0)
	{
	}
	// else allocs.tiny zone isnt null and theres no free_space so we need to create another mmap in allocs.tiny->next = mmap and bzero sizeof(t_zone)
	// to init the pointers of the new zone to 0
	else
	{
	}
	return ret;
}

void	*my_malloc(size_t size)
{
	if (size == 0)
		return NULL;
	if (size < TINY_SIZE)
	{ 
		create_tiny(size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
    return NULL;
}

// add better format here
void	hex_dump(void *s, size_t n)
{
    size_t i = 0;
    printf("Hex dump:\n");
    while (i < n){
        printf("%lu %x\n", i, ((char *)s)[i]);
        i++;
    }
    // printf("\n");
}

int main()
{
	char *five = my_malloc(5);
	// hex_dump(&allocs, sizeof(t_bucket));
	ft_bzero(&allocs, sizeof(t_bucket));
}
