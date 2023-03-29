#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUCKET_SIZE sizeof(t_bucket)
#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE getpagesize()

#define TINY_SIZE 240
#define SMALL_SIZE 1400 // should be more
#define LARGE_SIZE 5400 // should be more
#define ALLOCATIONS 105
#define TINY_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE // this can most likely be done better
#define SMALL_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE

// TODO
// TODO
// ADD tree afer each malloc with size and pointer to go through each time and do the full block in half the time
// add define for FREE & NOT FREE

typedef struct s_block
{
	struct s_block *next;
	struct s_block *prev;
	size_t size;
	size_t free; // padding // could add here pointer to the start of the zone, and check free with bitwise operator in free. wouldnt need to do (ptr > zone && ptr <= zone + TINY_ZONE_SIZE)
} t_block; // to check if block belogns to zone in tiny or small blocks

typedef struct s_zone
{
	struct s_zone *next;
	t_block *block;
	size_t free_space;
	size_t type; // padding // same as in tiny
} t_zone;

typedef struct s_bucket
{
	t_zone *tiny;
	t_zone *small;
	t_zone *large; // this should be something else as large dont have a special "zone", they dont have block (as their blocks dont have next / prev / free) nor free space or type?
} t_bucket;

t_bucket allocs;

void ft_bzero(void *s, size_t n)
{
	while (n--)
	{
		// ((char *)s)[n] = 125;
		// printf("%x", ((char *)s)[n]);
		((char *)s)[n] = '\0';
	}
}

// TODO
// what me gonna do with show_mem_alloc if its 16 each time?
// could keep size user gives me and malloc go_next_block anyway?
// and call get_next each time i need but keep in variable user size
size_t go_next_block(size_t size)
{
	return (size + 15) & ~15;
}

/*
	// create func that iterates through the zones and prints info about every zone, and block.
*/

void myfree(void *ptr)
{
	// if all blocks are free, then call munmap -> zone->free_space - ZONE_SIZE;
	// everytime free is called, check if prev is null & is free, if its not 'merge' with the block that is currently being freed
	// if ptr null then do nuthing
	// TODO forgot to check if im in the good zone, need to do some math if ptr < zone + ZONE_SIZE
	if (ptr == NULL)
		return;
	t_block *metadata = ptr - BLOCK_SIZE;
	t_zone *zone = NULL;
	if (metadata->size <= TINY_SIZE)
	{
		zone = allocs.tiny;
		printf("ptr %lu, zone %lu, math %lu, check %d\n", ptr, zone, (void *)zone + TINY_ZONE_SIZE, ptr <= (void *)zone + TINY_ZONE_SIZE);
		// the ptr needs to be inside the block so (ptr > zone && ptr <= zone + TINY_ZONE_SIZE)
		while (zone->next) // does this zone work? // this could break something i think if memory is in a weird 
		{
			if (ptr > zone && ptr <= zone + TINY_ZONE_SIZE) // this could be added directly to the while loop
				break;
			zone = zone->next;
		}
	}
	else if (metadata->size > TINY_SIZE && metadata->size <= SMALL_SIZE)
	{
		zone = allocs.small;
		printf("ptr %lu, zone %lu, check %d\n", ptr, zone, ptr < zone + SMALL_ZONE_SIZE);
	}
	else
	{
		// if its a large malloc call munmap and stop func here
		// zone = allocs.large;
		// call munmap directly and return
		return;
	}
	printf("ptr %lu meta %lu\n", ptr, metadata);
	metadata->free = 1;
	zone->free_space -= metadata->size;
	// this is to defrag
	// en que sentido intento el merge? primero a prev y dps a next?
	if (metadata->prev && metadata->prev->free)
	{
		// merge free blocks
		// if next is null does this work?
		t_block *prev = metadata->prev;
		t_block *next = metadata->next;
		prev->size += BLOCK_SIZE + metadata->size;
		prev->next = metadata->next;
		if (next)
			next->prev = prev;
		// metadata->next = ;
		//  metadata->free = 1;
		zone->free_space += (BLOCK_SIZE + metadata->size); // does this resta todo o solo parte?
		metadata = prev;
	}
	// does this die if prev was free? maybe as its not updated with new changes to linked list
	if (metadata->next && metadata->next->free)
	{
		// merge free blocks
		// if next next is null does this work?
		// t_block *prev = metadata->prev;
		t_block *after_next = metadata->next->next;
		t_block *next = metadata->next;
		metadata->next = after_next;
		metadata->size += BLOCK_SIZE + next->size;
		// next
		if (after_next)
		{
			after_next->prev = metadata;
			// prev->next = next;
		}
		// else
		// metadata->next
		// metadata->free = 1;
		zone->free_space += (BLOCK_SIZE + metadata->size); // does this resta todo o solo parte?
	}
	// free (munmap) if all blocks in a zone are free
	if (zone->free_space + ZONE_SIZE == TINY_SIZE)
	{
		// call munmap
	}
	else if (zone->free_space + ZONE_SIZE == SMALL_SIZE) // else if necessary
	{
		// call munmap
	}
	return;
}

// adapt this to use it with small zones too
void *create_tiny(size_t size)
{
	void *ret = NULL;
	t_zone *tiny = allocs.tiny;
	if (allocs.tiny == NULL)
	{
		// change allocs.tiny to tiny
		// rework this to be dynamic
		// create mmap of zone
		// aumentar pointer usando current como en el segundo else if, mas limpio q esta wea
		ret = mmap(NULL, TINY_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		printf("start of tiny block %lu\n", ret);
		allocs.tiny = ret;

		// init and populate the zone in another func?
		ft_bzero(allocs.tiny, TINY_ZONE_SIZE); // ft_bzero needed?

		// create metadata of zone
		allocs.tiny->next = NULL; // already done by ft_bzero
		allocs.tiny->block = ret + ZONE_SIZE;
		allocs.tiny->free_space = TINY_ZONE_SIZE - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size; // 2 block size cos theres the user block and the free block
		allocs.tiny->type = 0;																   // 0 is tiny 1 small? with define?
		printf("1 allocs.tiny %lu allocs.tiny->block %lu\n", allocs.tiny, allocs.tiny->block);

		// create block of size and return to user;
		allocs.tiny->block->prev = NULL; // already init with bzero
		allocs.tiny->block->next = ret + ZONE_SIZE + BLOCK_SIZE + size;
		allocs.tiny->block->size = size;
		allocs.tiny->block->free = 0; // 1 is free, 0 not free
		printf("2 allocs.tiny %lu allocs.tiny->block %lu\n", allocs.tiny->block, allocs.tiny->block->next);

		// create last free block
		allocs.tiny->block->next->prev = allocs.tiny->block; // already init with bzero
		allocs.tiny->block->next->next = NULL;				 //
		allocs.tiny->block->next->size = TINY_ZONE_SIZE - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size;
		allocs.tiny->block->next->free = 1; // 1 is free, 0 not free
		printf("3 free size after the metadata %lu allocs.tiny->block %lu\n", allocs.tiny->block->next + 1, allocs.tiny->block->next->next);

		printf("ret %lu\n", allocs.tiny->block + 1);
		return (allocs.tiny->block + 1);
		// printf("%lu\n", allocs.tiny->block->next->size);
	}
	// else if allocs.tiny isnt null and has free_space - (BLOCK_SIZE + size) > 0
	else if (allocs.tiny && allocs.tiny->free_space - BLOCK_SIZE - size > 0)
	{
		t_block *current = allocs.tiny->block;
		void *ret = NULL;
		while (current)
		{
			if (current->free == 1 && current->size >= BLOCK_SIZE + size)
			{
				// create block here
				printf("im here %lu c size %lu\n", current, current->size);
				// current->prev // prev is the same as up?
				current->next = (void *)current + BLOCK_SIZE + size;
				current->size = size;
				current->free = 0;
				ret = current + 1;

				printf("im here %lu c size %lu\n", current, current->size);
				current = current->next;

				// create free block
				current->prev = (void *)current - BLOCK_SIZE - size;
				current->next = NULL;
				// current->next->size = allocs.tiny->free_space - BLOCK_SIZE - size;
				current->size = allocs.tiny->free_space - BLOCK_SIZE - size;
				current->free = 1;
				printf("im here %lu c size %lu\n", current, current->size);
				return ret; // or current
							// break;
			}
			printf("current %lu prev %lu\n", current, current->prev);
			printf("free space tiny %lu, free block size + BLOCK_SIZE %lu\n", tiny->free_space, current->size);
			current = current->next;
		}
	}
	// if theres enough free_space but the memory is fragmented then it should create another zone, for now doesnt, should be added here?
	// else new block here
	// test this
	// printf("%lu\n", allocs.tiny->block->next->size);
	// else allocs.tiny zone isnt null and theres no free_space so we need to create another mmap in allocs.tiny->next = mmap and bzero sizeof(TINY_ZONE_SIZE)
	// to init the pointers of the new zone to 0
	t_zone *zone_current = allocs.tiny;
	while (zone_current->next)
		zone_current = zone_current->next;

	zone_current->next = mmap(NULL, TINY_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	zone_current = zone_current->next;
	printf("start of tiny block %lu\n", zone_current);

	// init and populate the zone in another func?

	// create metadata of zone
	zone_current->next = NULL;
	zone_current->block = ret + ZONE_SIZE;
	zone_current->free_space = TINY_ZONE_SIZE - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size;
	zone_current->type = 0; // 0 is tiny 1 small? with define?
	printf("1 allocs.tiny %lu allocs.tiny->block %lu\n", zone_current, zone_current->block);

	// create block of size and return to user;
	zone_current->block->prev = NULL; // already init with bzero
	zone_current->block->next = ret + ZONE_SIZE + BLOCK_SIZE + size;
	zone_current->block->size = size;
	zone_current->block->free = 0; // 1 is free, 0 not free
	printf("2 allocs.tiny %lu allocs.tiny->block %lu\n", zone_current->block, zone_current->block->next);

	// create last free block
	zone_current->block->next->prev = zone_current->block; // already init with bzero
	zone_current->block->next->next = NULL;				   //
	zone_current->block->next->size = TINY_ZONE_SIZE - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size;
	zone_current->block->next->free = 1; // 1 is free, 0 not free
	printf("3 free size after the metadata %lu allocs.tiny->block %lu\n", zone_current->block->next + 1, zone_current->block->next->next);

	printf("ret %lu\n", zone_current->block + 1);
	return (zone_current->block + 1);
}

void *my_malloc(size_t size)
{
	void *ret = NULL;
	if (size == 0)
		return NULL;
	size = go_next_block(size);
	if (size <= TINY_SIZE)
	{
		ret = create_tiny(size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	return ret;
}

// add better format here
// add itoa base here to transform to hex here? and write or printf %x
void hex_dump(void *s, size_t n)
{
	// reading byte by byte now needs to be displayed in hex, 0A for 10 ff for 255, etc
	size_t i = 0;
	printf("Hex dump of %lu:\n", s);
	while (i < n)
	{
		printf("%lu %d\n", i, ((char *)s)[i]);
		i++;
	}
	// printf("\n");
}

int toggleBit(int n, int k)
{
    return (n ^ (1 << (k - 1)));
}

void printBits(long num)
{
   for(int bit=0;bit<(sizeof(unsigned int) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
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
	printf("first %lu sec %lu\n", five, one);
	myfree(one);
	printf("page size %d %lu %lu\n", PAGE, TINY_ZONE_SIZE, TINY_ZONE_SIZE/ 240);
	//  ft_bzero(&allocs, sizeof(t_bucket));
	// toogle byte!
	int tst = -2147483648;
	printf("max %lu %d\n", tst, (int) tst);
	// tst ^= (1u << 31);

	// tst ^= (1u << 31);
	// tst ^= (1u << 30);
	// tst ^= (1u << 29);
	printf("max %lu %d\n", tst, (int) tst);
	printBits(tst);
}
