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
#define TINY_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE // this can most likely be done better // add TINY_ZONE_SIZE_SIZE?
#define SMALL_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE

// TODO
// ADD tree afer each malloc with size and pointer to go through each time and do the full block in half the time
// add define for FREE & NOT FREE

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
	t_zone *large; // this should be something else as large dont have a special "zone", they dont have block (as their blocks dont have next / prev / free) nor free space or type?
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
	t_zone *tiny = allocs.tiny;
	if (allocs.tiny == NULL)
	{
		// change allocs.tiny to tiny
		// rework this to be dynamic
		// create mmap of zone
		ret = mmap (NULL, TINY_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		printf("start of tiny block %lu\n", ret);
		allocs.tiny = ret;

		// init and populate the zone in another func?
		ft_bzero(allocs.tiny, TINY_ZONE_SIZE); // ft_bzero needed?

		// create metadata of zone
		allocs.tiny->next = NULL; // already done by ft_bzero
		allocs.tiny->block = ret + ZONE_SIZE;
		allocs.tiny->free_space = TINY_ZONE_SIZE - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size; // 2 block size cos theres the user block and the free block
		allocs.tiny->type = 0; // 0 is tiny 1 small? with define?
		printf("1 allocs.tiny %lu allocs.tiny->block %lu\n", allocs.tiny, allocs.tiny->block);

		// create block of size and return to user;
		allocs.tiny->block->prev = NULL; // already init with bzero
		allocs.tiny->block->next = ret + ZONE_SIZE + BLOCK_SIZE + size;
		allocs.tiny->block->size = size;
		allocs.tiny->block->free = 0; // 1 is free, 0 not free
		printf("2 allocs.tiny %lu allocs.tiny->block %lu\n", allocs.tiny->block, allocs.tiny->block->next);

		// create last free block
		allocs.tiny->block->next->prev = allocs.tiny->block; // already init with bzero
		allocs.tiny->block->next->next = NULL; //
		allocs.tiny->block->next->size = TINY_ZONE_SIZE - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size;
		allocs.tiny->block->next->free = 1; // 1 is free, 0 not free
		printf("3 free size after the metadata %lu allocs.tiny->block %lu\n", allocs.tiny->block->next + 1, allocs.tiny->block->next->next);

		return (allocs.tiny->block + 1);
		// printf("%lu\n", allocs.tiny->block->next->size);
	}
	// else if allocs.tiny isnt null and has free_space - (BLOCK_SIZE + size) > 0
	else if (allocs.tiny && allocs.tiny->free_space - BLOCK_SIZE - size > 0)
	{
		t_block *current = allocs.tiny->block;
		while (current)
		{
			if (current->free == 1 && current->size >= BLOCK_SIZE + size) 
			{
				// create block here
			}
			// if theres enough free_space but the memory is fragmented then it should create another zone, for now doesnt, should be added here?
			// theres something wrong with the size of this shit, im counting the size of the free block like used space when i shouldnt somewhere
			printf("current %lu prev %lu\n", current, current->prev);
		printf("free space tiny %lu, free block size + BLOCK_SIZE %lu\n", tiny->free_space, current->size );

			current = current->next;
		}
	}
	// else allocs.tiny zone isnt null and theres no free_space so we need to create another mmap in allocs.tiny->next = mmap and bzero sizeof(TINY_ZONE_SIZE)
	// to init the pointers of the new zone to 0
	else
	{
	}
	return ret;
}

void	*my_malloc(size_t size)
{
	void *ret = NULL;
	if (size == 0)
		return NULL;
	size = go_next_block(size);
	if (size < TINY_SIZE)
	{ 
		ret = create_tiny(size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
    return ret;
}

// add better format here
// add itoa base here to transform to hex here? and write or printf %x
void	hex_dump(void *s, size_t n)
{
	// reading byte by byte now needs to be displayed in hex, 0A for 10 ff for 255, etc
    size_t i = 0;
    printf("Hex dump of %lu:\n", s);
    while (i < n) {
        printf("%lu %d\n", i, ((char *)s)[i]);
        i++;
    }
    // printf("\n");
}

int main()
{
	size_t size = 5;
	char *five = my_malloc(size);
	for (size_t i = 0; i < size; ++i)
	{
		// printf("%lu\n", i);
		five[i] = 'a';
	}
	// hex_dump(five, size);
	// hex_dump(&allocs, 8);
	char *one = my_malloc(size);
	printf("page size %d %lu\n", PAGE, TINY_ZONE_SIZE / 240);
	// ft_bzero(&allocs, sizeof(t_bucket));
}