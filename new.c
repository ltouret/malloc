#include <stdio.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

// TODO move defines to another file, and utils, and bonus(?)
//  remember to make Makefile and check with 42 flags
// and check if malloc(0) does really malloc something if so change if size == 0 in my_malloc function
//! add getrlimit

#define BUCKET_SIZE sizeof(t_bucket) //! kind of useless
#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE sysconf(_SC_PAGESIZE)

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
// create func that iterates through the zones and prints info about every zone, and block.
//! this is my old bug -> in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block

typedef struct s_block
{
	struct s_block *next;
	struct s_block *prev;
	char *data; //? or void???
	unsigned int size;
	unsigned int free; // padding // could add here pointer to the start of the zone, and check free with bitwise operator in free. wouldnt need to do (ptr > zone && ptr <= zone + TINY_ZONE_SIZE)
} t_block;			   // to check if block belogns to zone in tiny or small blocks

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
	// t_zone *large; //! this should be something else as large dont have a special "zone", they dont have block (as their blocks dont have next / prev / free) nor free space or type?
} t_bucket;

t_bucket allocs;

// TODO
// if called on max size_t this returns 0 check it out -> compare to malloc
// change name? align_memory?
size_t go_next_block(size_t size)
{
	return (size + 15) & ~15;
}

// TODO
//! forgot to change allocs.tiny, small or large to null if all is freed so this does segfault if i free all and try to use my_malloc again
//? change zone to ** pointer to not need allocs.tiny or small here?
void my_free(void *ptr)
{
}

void create_block(t_block *ptr, t_block *prev, t_block *next, size_t size, size_t free)
{
	ptr->prev = prev;
	ptr->next = next;
	ptr->size = size;
	ptr->free = free;
}

//! OLD BUG -> in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
void *create_zone(size_t type_zone_size)
{
	printf("create_zone with size: %d\n", type_zone_size);
	t_zone *zone = mmap(NULL, type_zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); //! -1 here bc of MAP_ANONYMOUS
	if (zone == MAP_FAILED)
	{
		printf("Second malloc failed\n");
		return NULL;
	}
	zone->next = NULL;
	zone->block = (void *)zone + BLOCK_SIZE;
	zone->free_space = 0; //! change me to real data

	//! do just one of these and create a ugly init function
	zone->next = NULL;
	zone->block = (void *)zone + BLOCK_SIZE;
	zone->free_space = 0; //! change me to real data

	return zone;

	// create_block(ptr->block, NULL, (void *)ptr->block + BLOCK_SIZE + size, size, NOTFREE);
	// create_block(ptr->block->next, ptr->block, NULL, ptr->free_space, FREE);  //! dont add the new block with the free zone
}

// TODO
// malloc 3 tiny, 2 small at the start of the program?
void *create_tiny_small(t_zone **zone, size_t size, size_t type_zone_size)
{
	/*
	Iterate through zones to check if theres enough space for data:
	1 -> found a zone where there is enough, add data
	2 -> no zone has enough space, create zone, add data
	*/
	t_zone *current = *zone;

	//! this while loop can be done better ti think its 2 am
	while (current && size + BLOCK_SIZE >= current->free_space)
	{
		//! should be erased
		// if (size + BLOCK_SIZE < current->free_space)
		// {
		// break;
		// }
		current = current->next;
	}
	if (!current)
	{
		// t_zone *first = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); //! -1 here bc of MAP_ANONYMOUS
		// if (first == MAP_FAILED)
		// {
		// 	printf("Second malloc failed\n");
		// 	return NULL;
		// }
		current = create_zone(type_zone_size);
		// create_zone_plus_block(current, type_zone_size - ZONE_SIZE);
	}
}

// TODO
//! create large needs to be a multiple of page_size too!
//! if malloc size < 0 malloc returns a ptr address 0 -> its undefined behavior so idc
//! if malloc size == 0 malloc returns a ptr size 16 in m1 and 24 on ocean -> so create a allocs.tiny zone of 16
// Change this so that we call create_tiny_small and large in one function and only once, like for now we call create_tiny a lot, could call only once if change this shit
void *my_malloc(size_t size)
{
	void *ret = NULL;

	// if (size == 0) //! is this necessary, does malloc do this check? its diff on each fucking pc wtf
	// size++;
	if (!size)
		return NULL;			//! or this?
	size = go_next_block(size); //! check what to do if max size_t
	printf("size %zu: ", size);
	if (size <= TINY_SIZE)
	{
		// const size_t type_zone_size = TINY_ZONE_SIZE; //! erase me later when erase printf
		// printf("me tiny %lu %d\n", allocs.tiny, size, type_zone_size);
		ret = create_tiny_small(&allocs.tiny, size, TINY_ZONE_SIZE * 2); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else if (size <= SMALL_SIZE)
	{
		// const size_t type_zone_size = SMALL_ZONE_SIZE; //! erase me later when erase printf
		// printf("me smoll %lu %d\n", allocs.small, size, type_zone_size);
		ret = create_tiny_small(&allocs.small, size, SMALL_ZONE_SIZE * 2); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else
	{
		// ????
		//! Write a function to allocate large zones.
		printf("me large %d\n", size);
	}
	return ret;
}

//! remove me later
// #define DEBUG
#include <string.h>
#include <malloc.h>

int main()
{
	printf("tiny %d small %d \n", TINY_ZONE_SIZE, SMALL_ZONE_SIZE);
	my_malloc(10);
	if (0)
	{
		char *s = malloc(0);
		s[0] = '1';
		s[1] = '1';
		s[2] = '\0';
		printf("%s\n", s);
		struct rlimit old_lim, lim, new_lim;
		printf("page_size %d\n", sysconf(_SC_PAGESIZE));
		if (getrlimit(RLIMIT_AS, &old_lim) == 0)
			printf("Old limits -> soft limit= %ld \t"
				   " hard limit= %ld \n",
				   old_lim.rlim_cur, old_lim.rlim_max);
		if (getrlimit(RLIMIT_DATA, &old_lim) == 0)
			printf("Old limits -> soft limit= %ld \t"
				   " hard limit= %ld \n",
				   old_lim.rlim_cur, old_lim.rlim_max);
	}
}
