#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUCKET_SIZE sizeof(t_bucket)
#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE getpagesize()

#define TINY_SIZE 256
#define SMALL_SIZE 4096 // should be more
// #define LARGE_SIZE 5400 // should be more
#define ALLOCATIONS 105
#define TINY_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE
#define SMALL_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE) + 1) * PAGE
#define FREE 1
#define NOTFREE 0

// TODO
// defines should be other thing, maybe an struct with all the data? or call init func that has it somewheeeeere?
// ADD tree after each malloc with size and pointer to go through each time and do the full block in half the time

typedef struct s_block
{
	struct s_block *next;
	struct s_block *prev;
	size_t size;
	size_t free; // padding // could add here pointer to the start of the zone, and check free with bitwise operator in free. wouldnt need to do (ptr > zone && ptr <= zone + TINY_ZONE_SIZE)
} t_block;		 // to check if block belogns to zone in tiny or small blocks

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
// what me gonna do with show_mem_alloc if its 16 each time? -> this is the behavior of normal malloc all good
// could keep size user gives me and malloc go_next_block anyway?
// and call get_next each time i need but keep in variable user size
// if called on max size_t this returns 0 check it out
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
		// printf("tiny ptr %lu, zone %lu, math %lu, check %d\n", ptr, zone, (void *)zone + TINY_ZONE_SIZE, ptr <= (void *)zone + TINY_ZONE_SIZE);
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
		// printf("smoll ptr %lu, zone %lu, check %d\n", ptr, zone, ptr < zone + SMALL_ZONE_SIZE);
		while (zone->next) // does this zone work? // this could break something i think if memory is in a weird
		{
			if (ptr > zone && ptr <= zone + TINY_ZONE_SIZE) // this could be added directly to the while loop
				break;
			zone = zone->next;
		}
	}
	else
	{
		// if its a large malloc call munmap and stop func here
		// zone = allocs.large;
		// call munmap directly and return
		printf("Me large and all free\n");
		return;
	}
	metadata->free = 1;
	zone->free_space += metadata->size + BLOCK_SIZE;
	// this is to defrag
	// en que sentido intento el merge? primero a prev y dps a next?
	if (metadata->prev && metadata->prev->free)
	{
		// merge free blocks
		// if next is null does this work?
		t_block *prev = metadata->prev;
		t_block *next = metadata->next;
		prev->next = metadata->next;
		prev->size += BLOCK_SIZE + metadata->size;
		if (next)
			next->prev = prev;
		// metadata->next = ;
		//  metadata->free = 1;
		// TODO this is bad
		// zone->free_space += (BLOCK_SIZE + metadata->size); // does this resta todo o solo parte?
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
		// TODO this is bad
		// zone->free_space += (BLOCK_SIZE + next->size); // does this resta todo o solo parte?
	}
	// free (munmap) if all blocks in a zone are free
	// TODO this two if fail free_space + 32 will never be 240 fix WUT?
	if (zone->free_space + ZONE_SIZE + BLOCK_SIZE == TINY_ZONE_SIZE)
	{
		// add protection if munmap -1?
		// if ret == MAP_FAILED then return -1 and stop program
		int n = munmap((void *)metadata - ZONE_SIZE, TINY_ZONE_SIZE);
		if (n == -1)
		{
			//! error here
			printf("Free ERROR: All tiny\n");
		}
		else
			printf("Free OK: All tiny\n");
		// printf("n %d\n", n);
		// call munmap
	}
	else if (zone->free_space + ZONE_SIZE + BLOCK_SIZE == SMALL_ZONE_SIZE)
	{
		// call munmap
		int n = munmap((void *)metadata - ZONE_SIZE, SMALL_ZONE_SIZE);
		if (n == -1)
		{
			//! error here
			printf("Free ERROR: All smoll\n");
		}
		else
			printf("Free OK: All smoll\n");
	}
	return;
}

void create_zone(t_zone *ptr, size_t free_space)
{
	ptr->next = NULL;
	ptr->block = (void *)ptr + BLOCK_SIZE;
	ptr->free_space = free_space;
	// ptr->type = 0; //! this is useless
}

//! in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
void create_block(t_block *ptr, t_block *prev, t_block *next, size_t size, size_t free)
{
	ptr->prev = prev;
	ptr->next = next;
	ptr->size = size;
	ptr->free = free;
}

//! in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
void create_zone_plus_block(t_zone *ptr, size_t size, size_t free_space)
{
	// create metadata of zone
	create_zone(ptr, free_space);
	// create block of size and return to user;
	create_block(ptr->block, NULL, (void *)ptr->block + BLOCK_SIZE + size, size, NOTFREE);
	// create last free block
	create_block(ptr->block->next, ptr->block, NULL, ptr->free_space, FREE);
}

// TODO
// cut this into smaller funcs
// rename this func?
//! in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
void *create_tiny_small(t_zone **zone, size_t size, size_t type_zone_size)
{
	t_zone *copy = *zone;
	if (copy == NULL)
	{
		copy = mmap(NULL, type_zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (copy == MAP_FAILED)
		{
			printf("First malloc failed\n");
			return NULL;
		}
		*zone = copy;
		create_zone_plus_block(copy, size, type_zone_size - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size);
		printf("first %zu\n", size);
		return ((void *)copy->block + BLOCK_SIZE);
	}
	//! this could be >= to zero maybe need to test after cleaning -> the last block could work with 0 to zero maybe? need to test!
	//? rework this part?
	//! llego al final del block y no queda espacio para hcer el block de free se rompe no?
	else if (copy && copy->free_space >= BLOCK_SIZE + size)
	{
		t_block *current = copy->block;
		while (current)
		{
			//? current->size >= size + BLOCK_SIZE cos im creating a new block so i need at least BLOCK_SIZE + size of free memory
			if (current->free == FREE && current->size >= size + BLOCK_SIZE)
			{
				// create block here
				create_block(current, current->prev, (void *)current + BLOCK_SIZE + size, size, NOTFREE);

				// create free block
				create_block(current->next, current, NULL, copy->free_space - BLOCK_SIZE - size, FREE);

				// update zone free space
				//! check this could be wrong, or theres a better way to protect if theres no last free block at the end of the zone
				//? maybe cant go here cos protection of current->size - BLOCK_SIZE means we will never get in part of the code if theres no space to malloc BLOCK_SIZE
				//! try this and let just enough space to add a last free block of size 0 to check if last if is useful
				// if (current->next)
				copy->free_space = current->next->size; //? this is the true one if need to erase
				printf("curr %zu %zu next %zu %zu %zu\n", current, current->size,current->next, current->next->size, copy->free_space);
				// else
					// copy->free_space = 0;
				return ((void *)current + BLOCK_SIZE);
			}
			current = current->next;
		}
		return NULL; //! kind of catch all? if fails then return NULL
	}
	else
	{
		//? allocs.tiny zone isnt null and theres no free_space so we need to create another mmap in allocs.tiny->next = mmap and bzero sizeof(TINY_ZONE_SIZE)
		while (copy->next)
			copy = copy->next;

		copy->next = mmap(NULL, type_zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (copy->next == MAP_FAILED)
		{
			printf("Second malloc failed\n");
			return NULL;
		}
		copy = copy->next;
		create_zone_plus_block(copy, size, type_zone_size - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size);
		printf("new zone %zu\n", size);
		return ((void *)copy->block + BLOCK_SIZE);
	}
}

//TODO
//! if malloc size < 0 malloc returns a ptr address 0 -> its undefined behavior so idc
//! if malloc size == 0 malloc returns a ptr size 16 in m1 and 24 on ocean -> so create a allocs.tiny zone of 16
void *my_malloc(size_t size)
{
	void *ret = NULL;

	if (size == 0)
		size++;
	size = go_next_block(size); //! check what to do if max size_t
	if (size <= TINY_SIZE)
	{
		const size_t type_zone_size = TINY_ZONE_SIZE; //! erase me later when erase printf
		// printf("me tiny %lu %d\n", allocs.tiny, size, type_zone_size);
		ret = create_tiny_small(&allocs.tiny, size, type_zone_size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else if (size <= SMALL_SIZE)
	{
		const size_t type_zone_size = SMALL_ZONE_SIZE; //! erase me later when erase printf
		// printf("me smoll %lu %d\n", allocs.small, size, type_zone_size);
		ret = create_tiny_small(&allocs.small, size, type_zone_size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else
	{
		// ????
		//! Write a function to allocate large zones.
		printf("me large %d\n", size);
	}
	return ret;
}

//! idea : code malloc_usable_size ez af and ez bonus!
size_t my_malloc_usable_size(void *ptr)
{
	t_block *block = (void *)ptr - BLOCK_SIZE;
	// printf("%zu\n", block->size);
	return (block->size);
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
	for (int bit = 0; bit < (sizeof(unsigned int) * 8); bit++)
	{
		printf("%i ", num & 0x01);
		num = num >> 1;
	}
}

//! remove me later on
// #define DEBUG
#include <string.h>
#ifndef __APPLE__
#include <malloc.h>
#endif

int main()
{
	#ifdef DEBUG
	{
		void *p = malloc(0);
		#ifdef __APPLE__
		size_t block_size = malloc_size(p);
		#else
		size_t block_size = malloc_usable_size(p);
		#endif
		size_t alignment_padding = block_size;

		printf("Machine word size: %lu bytes\n", sizeof(void *));
		printf("Alignment of malloc: %lu bytes\n", alignment_padding);

		free(p);
		return 0;
	}
	#endif
	// size_t size = 5;
	// char *one = my_malloc(size);
	// char *two = my_malloc(size);
	// // char *three = my_malloc(241);
	// myfree(one);
	// myfree(two);
	// // myfree(three);
	//! try this and let just enough space to add a last free block of size 0 to check if last if is useful
	if (1)
	{
		size_t allo = (TINY_ZONE_SIZE - ZONE_SIZE) / (TINY_SIZE + BLOCK_SIZE);
		size_t size = 256;
		char *arr[allo + 1];
		for (size_t i = 0; i < allo; i++)
		{
			// size = 256;
			// arr[i] = my_malloc(size);
			// memset(arr[i], '0', size - 1);
			// my_free(arr[i]);
			size = 256;
			arr[i] = my_malloc(size);
			// arr[i] = my_malloc(size);
			memset(arr[i], '0', size - 1);
		}
		// myfree(arr[allo - 1]);
		// arr[allo - 1] = my_malloc(size); //! change this to 128 to test line 390
		arr[allo] = my_malloc(128); //! change this to 128 to test line 390
		// my_malloc(16); //!comment this
	}
	printf("page size %d %lu %lu\n", PAGE, TINY_ZONE_SIZE, (TINY_ZONE_SIZE - ZONE_SIZE) / (TINY_SIZE + BLOCK_SIZE));
	printf("page size %d %lu %lu\n", PAGE, SMALL_ZONE_SIZE, (SMALL_ZONE_SIZE - ZONE_SIZE) / (SMALL_SIZE + BLOCK_SIZE));
	// ft_bzero(&allocs, sizeof(t_bucket));
	// toogle byte!
	// int tst = -2147483648;
	// printf("max %lu %d\n", tst, (int) tst);
	// tst ^= (1u << 31);

	// tst ^= (1u << 31);
	// tst ^= (1u << 30);
	// tst ^= (1u << 29);
	// printf("max %lu %d\n", tst, (int) tst);
	// printBits(tst);
}
