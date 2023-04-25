#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

//TODO move defines to another file, and utils, and bonus(?)
// remember to make Makefile and check with 42 flags

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

// TODO
//? for large erase this, and just have them floating aroung with a block, zone is useless as they are just FREE or NOTFREE and cant be reused once freed
typedef struct s_bucket
{
	t_zone *tiny;
	t_zone *small;
	t_zone *large; //! this should be something else as large dont have a special "zone", they dont have block (as their blocks dont have next / prev / free) nor free space or type?
} t_bucket;

t_bucket allocs;

//? kind of useless now?
void ft_bzero(void *s, size_t n)
{
	while (n--)
	{
		((char *)s)[n] = '\0';
	}
}

// TODO
// if called on max size_t this returns 0 check it out -> compare to malloc
size_t go_next_block(size_t size)
{
	return (size + 15) & ~15;
}

/*
	// create func that iterates through the zones and prints info about every zone, and block.
*/

//TODO
//! forgot to change allocs.tiny, small or large to null if all is freed so this does segfault if i free all and try to use my_malloc again
//? change zone to ** pointer to not need allocs.tiny or small here?
void my_free(void *ptr)
{
	// if all blocks are free, then call munmap -> zone->free_space - ZONE_SIZE;
	// everytime free is called, check if prev is null & is free, if its not 'merge' with the block that is currently being freed
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
		//! can be done in only one loop without if most likely
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
	metadata->free = FREE;
	printf("me free %zu\n", metadata->size);
	zone->free_space += metadata->size;
	printf("me free %zu\n", zone->free_space);
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
		zone->free_space += BLOCK_SIZE; // does this resta todo o solo parte?
		metadata = prev;
		printf("free prev\n");
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
		zone->free_space += BLOCK_SIZE; // does this resta todo o solo parte?
		printf("free next, %zu %zu\n", zone->free_space, metadata->size);
		// zone->free_space += (BLOCK_SIZE + next->size); // does this resta todo o solo parte?
	}
	// free (munmap) if all blocks in a zone are free
	// TODO this two if fail free_space + 32 will never be 240 fix WUT?
	//! now that i changed free_space relation with blocks this two most likely are dead
	//! if theres no free block at the end this fails too cos free_space + BLOCK_SIZE would be bigger than zone size
	if (zone->free_space + ZONE_SIZE + BLOCK_SIZE == TINY_ZONE_SIZE)
	{
		// add protection if munmap -1?
		// if ret == MAP_FAILED then return -1 and stop program
		int n = munmap((void *)metadata - ZONE_SIZE, TINY_ZONE_SIZE);
		if (n == -1)
		{
			//! error here
			printf("Free ERROR: All tiny\n");
			return;
		}
		else
			printf("Free OK: All tiny\n");
		allocs.tiny = NULL;
		// printf("n %d\n", n);
		// call munmap
	}
	//! now that i changed free_space relation with blocks this two most likely are dead
	//! if theres no free block at the end this fails too cos free_space + BLOCK_SIZE would be bigger than zone size
	else if (zone->free_space + ZONE_SIZE + BLOCK_SIZE == SMALL_ZONE_SIZE)
	{
		// call munmap
		int n = munmap((void *)metadata - ZONE_SIZE, SMALL_ZONE_SIZE);
		if (n == -1)
		{
			//! error here
			printf("Free ERROR: All smoll\n");
			return;
		}
		else
			printf("Free OK: All smoll\n");
		allocs.small = NULL;
	}
	return;
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
	ptr->next = NULL;
	ptr->block = (void *)ptr + BLOCK_SIZE;
	ptr->free_space = free_space;
	create_block(ptr->block, NULL, (void *)ptr->block + BLOCK_SIZE + size, size, NOTFREE);
	create_block(ptr->block->next, ptr->block, NULL, ptr->free_space, FREE);
}

// TODO
// change name of copy to current_zone 
//? replace copy for *zone -> not possible need to keep first pointer so rename
// cut this into smaller funcs
// rename this func?
//! in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
// ! the first and last in the if case can be on the same block, if i move the second if on top and merge the last two
void *create_tiny_small(t_zone **zone, size_t size, size_t type_zone_size)
{
	t_zone *copy = *zone;
	//! this could be >= to zero maybe need to test after cleaning -> the last block could work with 0 to zero maybe? need to test!
	//? rework this part?
	//! llego al final del block y no queda espacio para hcer el block de free se rompe no?
	//! aqui esta el error, esto revisa solo el primer zone, si ese esta lleno crea otro, no revisa en copy->next
	while (copy && copy->free_space < size && copy->next) {
		// printf("copy %zu %zu ", copy->free_space, BLOCK_SIZE + size);
		copy = copy->next;
	}
	if (copy && copy->free_space >= size)
	{
		t_block *current = copy->block;
		while (current)
		{	
			if (current->free == FREE && current->size >= size)
				{
				const char size_flag = current->size >= size + BLOCK_SIZE;
				create_block(current, current->prev, (void *)current + BLOCK_SIZE + size, size, NOTFREE);
				if (size_flag) {
					copy->free_space -= BLOCK_SIZE;
					create_block(current->next, current, NULL, copy->free_space - size, FREE);
				}
				copy->free_space -= size;
				printf("curr %zu %zu next %zu %zu %zu\n", current, current->size,current->next, current->next->size, copy->free_space);
				return ((void *)current + BLOCK_SIZE);
			}
			current = current->next;
		}
		return NULL; //! kind of catch all? if fails then return NULL
	}
	else
	{
		t_zone *first = NULL;
		first = mmap(NULL, type_zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (first == MAP_FAILED)
		{
			printf("Second malloc failed\n");
			return NULL;
		}
		create_zone_plus_block(first, size, type_zone_size - ZONE_SIZE - BLOCK_SIZE - BLOCK_SIZE - size);
		if (copy == NULL)
			*zone = first;
		else
			copy->next = first;
		printf("new zone %zu\n", size);
		return ((void *)first->block + BLOCK_SIZE);
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
	printf("size %zu: ", size);
	if (size <= TINY_SIZE)
	{
		// const size_t type_zone_size = TINY_ZONE_SIZE; //! erase me later when erase printf
		// printf("me tiny %lu %d\n", allocs.tiny, size, type_zone_size);
		ret = create_tiny_small(&allocs.tiny, size, TINY_ZONE_SIZE); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else if (size <= SMALL_SIZE)
	{
		// const size_t type_zone_size = SMALL_ZONE_SIZE; //! erase me later when erase printf
		// printf("me smoll %lu %d\n", allocs.small, size, type_zone_size);
		ret = create_tiny_small(&allocs.small, size, SMALL_ZONE_SIZE); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else
	{
		// ????
		//! Write a function to allocate large zones.
		printf("me large %d\n", size);
	}
	return ret;
}

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

int toggleBit(int n, int k) //! useless
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

//TODO now
// free adds BLOCK_SIZE when it shouldnt
// por ahora acada vez q creo un bloque en una zona existente escribo el bloque de free al lado, que pasa cuando
// el bloque de al lado ya tiene algo, reescribo esa zona en free aunque esta ocupada x) cmt

int main()
{
	#ifdef DEBUG
	{
		void *p = malloc(4095);
		#ifdef __APPLE__
		size_t block_size = malloc_size(p);
		#else
		size_t block_size = malloc_usable_size(p);
		#endif

		printf("Machine word size: %lu bytes\n", sizeof(void *));
		printf("block_size of malloc: %lu bytes\n", block_size);

		free(p);
		return 0;
	}
	#endif
	// size_t size = 5;
	// char *one = my_malloc(size);
	// char *two = my_malloc(size);
	// // char *three = my_malloc(241);
	// my_free(one);
	// my_free(two);
	// // my_free(three);
	//! try this and let just enough space to add a last free block of size 0 to check if last if is useful
	if (1)
	{
		size_t allo = (TINY_ZONE_SIZE - ZONE_SIZE) / (TINY_SIZE + BLOCK_SIZE);
		size_t size = 256;
		char *arr[allo + 1];
		for (size_t i = 0; i < allo; i++)
		{
			{
				//! wut?
				size = 256;
				arr[i] = my_malloc(size);
				memset(arr[i], '0', size - 1);
				// my_free(arr[i]);
			}
			{
				// size = 256;
				// arr[i] = my_malloc(size);
				// arr[i] = my_malloc(size);
				// memset(arr[i], '0', size - 1);
			}
		}
		arr[allo] = my_malloc(128); //! change this to 128 to test line 390
		my_free(arr[allo - 2]);
		my_free(arr[allo - 3]);
		my_free(arr[allo - 1]);
		arr[allo] = my_malloc(256); //! change this to 128 to test line 390
		arr[allo] = my_malloc(256); //! change this to 128 to test line 390
		arr[allo] = my_malloc(112); //! change this to 128 to test line 390
		arr[allo] = my_malloc(32); //! change this to 128 to test line 390
		arr[allo] = my_malloc(32); //! change this to 128 to test line 390

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
