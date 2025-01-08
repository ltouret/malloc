#include <stdio.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <unistd.h>

// TODO move defines to another file, and utils, and bonus(?)
//  remember to make Makefile and check with 42 flags
// and check if malloc(0) does really malloc something if so change if size == 0 in my_malloc function
//! add getrlimit protection only once, if its -1 then fuck ittttt? or just ignore and check mmap != -1

#define BUCKET_SIZE sizeof(t_bucket) //! kind of useless
#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

#define TINY_SIZE PAGE_SIZE / 4 //! change this to 1/4 of pagesize and small to pagesize 
#define SMALL_SIZE PAGE_SIZE // should be more
// #define LARGE_SIZE 5400 // should be more
#define ALLOCATIONS 105
#define TINY_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE_SIZE) + 1) * PAGE_SIZE
#define SMALL_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE_SIZE) + 1) * PAGE_SIZE

//! change this two for an enum :)
#define FREE 1
#define NOTFREE 0

// TODO
// defines should be other thing, maybe an struct with all the data? or call init func that has it somewheeeeere?
// ADD tree after each malloc with size and pointer to go through each time and do the full block in half the time
// create func that iterates through the zones and prints info about every zone, and block.
//! this is my old bug -> in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
// void *create_large(t_zone **zone, size_t size) -> change to just *zone no need for double pointer, same for create tiny, mayvbe even erase as sometimes not needed

typedef enum zone
{
	TYPE_TINY,
	TYPE_SMALL,
	TYPE_LARGE
} e_zone;

typedef struct s_block
{
	struct s_block *next;
	struct s_block *prev;
	size_t user_size;
	// char *data; //? or void??? //! seems useless for now erase? -> maybe use when coding find_zone for free
	unsigned int size;
	unsigned int free; // padding // could add here pointer to the start of the zone, and check free with bitwise operator in free. wouldnt need to do (ptr > zone && ptr <= zone + TINY_ZONE_SIZE)
} t_block;			   // to check if block belogns to zone in tiny or small blocks

typedef struct s_zone
{
	struct s_zone *next;
	t_block *block;
	size_t free_space;
	e_zone type;
	int padding; // !padding maybe useless in certain machines -> test!
} t_zone;

// TODO
//? for large erase this, and just have them floating aroung with a block, zone is useless as they are just FREE or NOTFREE and cant be reused once freed
typedef struct s_bucket
{
	t_zone *zone;
	// t_zone *large; //! this should be something else as large dont have a special "zone", they dont have block (as their blocks dont have next / prev / free) nor free space or type?
} t_bucket;

t_bucket allocs;

void write_nb_base(unsigned int nb, unsigned int base)
{
    char string[10];
    int digit;
    int i;

    i = 0;
	if (nb == 0) {
        string[i] = '0';
		i++;
	}

    while (nb > 0)
    {
        digit = nb % base;
        if (digit > 9)
        {
            digit -= 10;
            string[i] = digit + 'A';
        }
        else
        {
            string[i] = digit + '0';
        }
        i++;

        nb /= base;
    }

    if (base == 16)
    {
        string[i] = 'x';
        i++;
        string[i] = '0';
        i++;
    }

    i--;
    while (i >= 0)
    {
        write(1, &(string[i]), 1);
        i--;
    }
}

size_t print_blocks(t_block *block)
{
	size_t counter = 0;
    while (block)
    {
		if (block->free == FREE) {
	        block = block->next;
			continue;
		}
        write_nb_base((unsigned long)block + BLOCK_SIZE, 16);
        write(1, " - ", 3);
        write_nb_base((unsigned long)block + BLOCK_SIZE + block->user_size, 16);
        write(1, " : ", 3);
        write_nb_base(block->user_size, 10);
        write(1, " bytes\n", 7);

		counter += block->user_size;
        block = block->next;
    }
	return counter;
}

void show_alloc_mem()
{
	t_zone *zone = allocs.zone;
	size_t counter = 0;
    while (zone)
    {
        switch (zone->type) {
            case TYPE_TINY:
                write(1, "TINY : ", 7);
                break;
            case TYPE_SMALL:
                write(1, "SMALL : ", 8);
                break;
            case TYPE_LARGE:
                write(1, "LARGE : ", 8);
                break;
        }

        write_nb_base((unsigned long)zone + ZONE_SIZE, 16);
        write(1, "\n", 1);

        counter += print_blocks(zone->block);
        zone = zone->next;
    }
    write(1, "Total : ", 8);
    write_nb_base(counter, 10);
    write(1, " bytes\n", 7);
}

//? rework this
size_t get_zone_size(e_zone type, size_t large_alloc)
{
	//! change this uglyyyy -> to switch
	size_t size = 0;
	if (type == TYPE_TINY) {
		size = TINY_ZONE_SIZE;
	} else if (type == TYPE_SMALL) {
		size = SMALL_ZONE_SIZE;
	} else {
		// size_t mask = ~(PAGE_SIZE - 1);
		// size = (large_alloc + (page_size - 1)) & mask;
  		size = (large_alloc + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
		printf("large malloc is: %ld\n", size);
		// ? change to big
		// size = SMALL_ZONE_SIZE; //! wrong here return LARGE changed to the closest PAGE SIZE
	}
	// printf("get_zone_size malloc is: type %d size: %ld\n", type, size);
	return size;
}

//? rework this
// get_zone_type + get_zone_size could be merged maybe:
// i mean they are always needed together so............
// return e_zone?
e_zone get_zone_type(size_t size)
{
	//! change this uglyyyy
	return (0 * (size <= TINY_SIZE)) +
		   (1 * ((size > TINY_SIZE) && (size <= SMALL_SIZE))) +
		   (2 * (size > SMALL_SIZE));
}

//? get zone of that block
void *get_zone_of_block(void *ptr, size_t type_zone_size) {
	t_zone *current = allocs.zone;

	while (current)
	{
		if ((void *)current <= ptr && (void *)current + type_zone_size > ptr) {
			printf("free: block found on zone %ld\n", current);
			break;
		}
		current = current->next;
	}
	return current;
}

void *get_block(t_block *ptr)
{
	t_zone *curr_zone = allocs.zone;
	while (curr_zone) {
		t_block *curr_block = curr_zone->block;
		while (curr_block)
		{
			// if (curr_block->free == FREE) {
				// break;
			// }
			if ((void *)ptr == (void *)curr_block + BLOCK_SIZE) {
				return ptr;
			}
			curr_block = curr_block->next;
		}
		curr_zone = curr_zone->next;
	}
	return NULL;
}

int count_zone_type(e_zone type) {
	t_zone *current = allocs.zone;
	int count = 0;

	while (current)
	{
		if (current->type == type) {
			count++;
		}
		current = current->next;
	}
	// printf("count type %d count %d ", type, count);
	return count;
}

void remove_zone(t_zone *zone) {
	t_zone *current = allocs.zone;
	t_zone *prev = NULL;

	if (current == zone) {
		allocs.zone = zone->next;
		return;
	} 
	while (current)
	{
		if (current == zone) {
			prev->next = current->next;
			break;
		}
		prev = current;
		// printf("free: zone %ld free space %ld should free %ld?\n", current, current->free_space, prev);
		current = current->next;
	}
}

void my_free(void *ptr) {
	// show_zone();
	if (ptr == NULL) {
		return;
	}
	if (get_block(ptr) == NULL) {
		return;
	}
	t_block *metadata = (void *)ptr - BLOCK_SIZE;
	if (metadata->free == FREE) {
		return;
	}
	//! get zone size could work with size as parameter this is bad
	e_zone zone_type = get_zone_type(metadata->size);
	size_t zone_size = get_zone_size(zone_type, 0);
	t_zone *zone = get_zone_of_block(ptr, zone_size);
	printf("free: block type %ld\n", zone_type);
	if (zone_type < TYPE_LARGE) {
		//! tiny - small, change flag to FREE, change free_space += size, if zone is all freed and theres more zone of that type then munmap
		metadata->free = FREE;
		zone->free_space += (metadata->size + BLOCK_SIZE);
		printf("free: zone %ld, metadata %ld free space %ld should free %ld?\n", zone, metadata, zone->free_space, zone->free_space == zone_size - ZONE_SIZE);
		int count_zone = count_zone_type(zone_type);
		if (count_zone > 1 && zone->free_space + ZONE_SIZE == zone_size) {
			//! call munmap
			// show_zone();
			remove_zone(zone);
			munmap((void *)zone, zone->free_space + ZONE_SIZE);
			// show_zone();
		}
	} else {
		//! call munmap - large
		zone = (void *)ptr - BLOCK_SIZE - ZONE_SIZE;
		printf("free: zone %ld free space %ld should free %ld?\n", zone, zone->free_space, zone->free_space == zone_size - ZONE_SIZE);
		remove_zone(zone);
		// show_zone();
		munmap((void *)zone, zone->free_space + ZONE_SIZE);
  	}	
}

// TODO
// if called on max size_t this returns 0 check it out -> compare to malloc
//! change name? align_memory?
size_t align_memory(size_t size)
{
	return (size + 15) & ~15;
}

// TODO
//? change zone to ** pointer to not need alloc

t_block *create_block(t_block *ptr, t_block *prev, t_block *next, size_t size,  size_t user_size, size_t free)
{
	ptr->prev = prev;
	ptr->next = next;
	ptr->size = size;
	ptr->user_size = user_size;
	ptr->free = free;
	return ptr;
}

void add_zone(t_zone *zone)
{
	t_zone *current = allocs.zone;
	printf("add zone %ld free space %ld\n", zone, zone->free_space);
	if (!current)
	{
		allocs.zone = zone;
		return;
	}
	while (current->next)
	{
		current = current->next;
	}
	current->next = zone;
}

//! OLD BUG -> in create_block if theres not enough space to create teh last free bock it will segfault add protections somewhere -> test what happens if at the end of the zone theres not enough space for a free block
// ? do i add get_zone_type here to add zone type directly here
void *create_zone(size_t size, e_zone zone_type)
{
	size_t zone_size = get_zone_size(zone_type, size); //! here need to get size of the zone i want to malloc, huw ? ******
	t_zone *zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); //! -1 here bc of MAP_ANONYMOUS
	if (zone == MAP_FAILED)
	{
		printf("create_zone failed, zone size %ld\n", zone_size);
		return NULL;
	}
	zone->next = NULL;
	zone->block = NULL; //! block to null seems a better idea, will test now, reverse if fails
	zone->type = zone_type;
	zone->free_space = zone_size - ZONE_SIZE;
	add_zone(zone);
	printf("create_zone with size: %ld and type %d\n", zone_size, zone->type);
	return zone;
}

void *create_large(t_zone **zone, size_t size, size_t user_size)
{
	t_zone *current = create_zone(size + BLOCK_SIZE, TYPE_LARGE);
	t_block *ptr = create_block((void *)current + ZONE_SIZE, NULL, NULL, size, user_size, NOTFREE);
	current->block = ptr;
	//! test this with print here
	printf("create large in zone %ld free space %ld block size %ld\n", current, current->free_space, ptr->size);
	return (void *)ptr + BLOCK_SIZE;
}

// TODO
//  cut this into smaller funcs, its frankenstein monster
// the logic to create the first block and push the last block maybe could be almost the same
//! void *create_tiny_small(t_zone **zone, size_t size) -> new func, could even erase **zone param... as it will be always be allocs.zone
void *create_tiny_small(t_zone **zone, size_t size, size_t user_size)
{
	/*
	Iterate through zones to check if theres enough space for data:
	1 -> found a zone where there is enough, add data
	2 -> no zone has enough space, create zone, add data
	*/
	//! change var name to curr_zone
	t_zone *current = *zone;
	size_t zone_type = get_zone_type(size); //! here need to get size of the zone i want to malloc, huw ? ******
	printf("type %ld ", zone_type);

	//! this while loop can be done better ti think its 2 am -> do i keep >= or keep it safe but ugly with > as with >= maybe i get out of space
	//! move this to its own function
	// check here when changing to one zone, and if zone is the type we need
	while (current)
	{
		if (current->type == zone_type && BLOCK_SIZE + size < current->free_space)
		{
			break;
		}
		current = current->next;
	}
	// printf("found enough space in zone %ld free space %ld\n", current, current->free_space);
	//! case 2 -> no zone has enough space, create zone, add data
	//? code the add data here for now doesnt!
	// printf("create zone first malloc found enough space in zone %ld free space %ld\n", current, current->type);
	if (!current)
	{
		//! this is wrong for now, when we change to one zone change ittttt
		// ? do another func that from the zone type returns the zone size?
		current = create_zone(size, zone_type);
		printf("first malloc zone enough space in zone %ld free space %ld %d\n", current, current->free_space, size + BLOCK_SIZE <= current->free_space);
		// return NULL;
		// create_zone_plus_block(current, type_zone_size - ZONE_SIZE);
	}
	//! case 1 -> !found a zone where there is enough, add data
	if (current && current->type == zone_type && size + BLOCK_SIZE <= current->free_space) //! the second condition is overkill as it used in the while to get the current free zone
	{
		// printf("found enough space in zone %ld free space %ld\n", current, current->free_space);

		//! what do i do if its the first block of the zone, example zone was created by init, theres no block info in ****

		t_block *curr_block = current->block;
		printf("found enough space in zone %ld free space %ld, block %ld\n", current, current->free_space, curr_block);


		// first block in zone
		if (!curr_block)
		{
			//? clean this
			t_block *ptr = create_block((void *)current + ZONE_SIZE, NULL, NULL, size, user_size, NOTFREE);
			current->block = ptr;
			// current->block = (void *)current + ZONE_SIZE;
			current->free_space -= (size + BLOCK_SIZE);
			//! test this with print here
			printf("first head malloc found enough space in zone %ld %ld size %ld\n", current, ptr, ptr->size);
			return (void *)ptr + BLOCK_SIZE;
		}
		// add here now the while loop to find the next free zone or null
		//? what happens if the last block is free and i want to write the data there, it wont go into the loop so could fail, edge case!!!
		while (curr_block->next)
		{
			//? maybe can add this condition in the loop directly
			if (curr_block->free == FREE && size <= curr_block->size) {
				break;
			}
			// printf("block %ld block size %ld free space %ld\n", curr_block, curr_block->size, current->free_space);
			curr_block = curr_block->next;
		}
		// else block exists and is free
		//! test this shit a fuckton
		if (curr_block->free == FREE && size <= curr_block->size) //? and if enough space
		{
			//? create update block?
			// t_block *ptr = create_block(curr_block, curr_block->prev, curr_block->next, size, NOTFREE);
			// curr_block->size = size; //?????? do i change this value
			curr_block->free = NOTFREE;
			curr_block->user_size = user_size;
			current->free_space -= (curr_block->size + BLOCK_SIZE);
			// curr_block->next = ptr; //? is this an error?
			printf("if free block malloc found enough space in zone %ld block size %ld free space %ld\n", current, curr_block->size, current->free_space);
			//! this shit fails hardcore, if the block was using 100 and now i use 40, and i change free_space to reflect this then
			//! it will seem like i have more free than i actually have as i lost 60 in the fragmentation!!!
			//! if i dont update free_space then it will show as if i have less memory i actually have, but its the only way with memory fragmentation
			//! so if not bonus then i cant update free_space here
			return (void *)curr_block + BLOCK_SIZE;
		}
		else
		{
			// if new block?
			//! create new block
			//? problem with new block prev
			t_block *ptr = create_block((void *)curr_block + curr_block->size + BLOCK_SIZE, curr_block, NULL, size, user_size, NOTFREE);
			curr_block->next = ptr;
			current->free_space -= (size + BLOCK_SIZE);
			printf("new block malloc %ld new block %ld prev %ld next %ld size %ld\n", curr_block, (void *)curr_block + curr_block->size + BLOCK_SIZE, 
				curr_block->next->prev, curr_block->next, curr_block->size);
			return (void *)ptr + BLOCK_SIZE;
		}
	}
	return NULL; //? catch all
}

//! create function that prints block data, and another that prints zone data.

// TODO
//! or return memory address
//? theres a better way to do this - rework
//! change protection to be called only once i forgot about that l000l
//? what do i return here, check in my_malloc if allocs.zone != NULL?
void *init()
{
	// code this better
	if (allocs.zone)
	{
		return allocs.zone;
	}
	t_zone *tiny = create_zone(TINY_ZONE_SIZE, TYPE_TINY);
	t_zone *tiny2 = create_zone(TINY_ZONE_SIZE, TYPE_TINY);
	if (tiny == MAP_FAILED || tiny2 == MAP_FAILED)
	{
		printf("tiny init failed, zone size %ld\n", TINY_ZONE_SIZE);
		return NULL;
	}

	// void add_zone(t_zone *zone)
	// ? call add zone instead of manually adding it
	// add_zone(tiny);
	// add_zone(tiny2);

	// allocs.zone = tiny;
	// allocs.zone->next = tiny2;
	// printf("pre malloc with tiny 1 %ld size %ld\n", allocs.tiny, allocs.tiny->free_space);
	// printf("pre malloc with tiny 2 %ld size %ld\n", allocs.tiny->next, allocs.tiny->next->free_space);

	t_zone *small = create_zone(SMALL_ZONE_SIZE, TYPE_SMALL);
	t_zone *small2 = create_zone(SMALL_ZONE_SIZE, TYPE_SMALL);
	if (small == MAP_FAILED || small2 == MAP_FAILED)
	{
		printf("small init failed, zone size %ld\n", SMALL_ZONE_SIZE);
		return NULL;
	}

	// add_zone(small);
	// add_zone(small2);

	// allocs.zone->next->next = small;
	// allocs.zone->next->next->next = small2;
	// printf("pre malloc with small 1 %ld size %ld\n", allocs.small, allocs.small->free_space);
	// printf("pre malloc with small 2 %ld size %ld\n", allocs.small->next, allocs.small->next->free_space);
	return allocs.zone;
}

// TODO
//! create large needs to be a multiple of page_size too!
//! if malloc size < 0 malloc returns a ptr address 0 -> its undefined behavior so idc
//! if malloc size == 0 malloc returns a ptr size 16 in m1 and 24 on ocean -> so create a allocs.tiny zone of 16
// Change this so that we call create_tiny_small and large in one function and only once, like for now we call create_tiny a lot, could call only once if change this shit
void *my_malloc(size_t size)
{
	void *ret = NULL;
	size_t user_size = size;

	// NULL may also be returned by a successful call to malloc() with a size of zero
	if (size == 0)
		return NULL;
	if (init() == false) //? change to new *zone
		return NULL;
	// if (size == 0) //! is this necessary, does malloc do this check? its diff on each fucking pc wtf
	// size++;
	size = align_memory(size); //! check what to do if max size_t
	printf("asked malloc of size %zu: %ld \n", size, user_size);
	//! change this remove ifs, only call create_malloc
	if (size > SMALL_SIZE)
	{
		// const size_t type_zone_size = TINY_ZONE_SIZE; //! erase me later when erase printf
		// printf("me tiny %lu %d\n", allocs.tiny, size, type_zone_size);
		printf("me large %ld\n", size);
		ret = create_large(&allocs.zone, size, user_size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	else
	{
		//! Write a function to allocate large zones.
		printf("me smol %ld\n", size);
		ret = create_tiny_small(&allocs.zone, size, user_size); // check if tiny doesnt exist if it does check tehres space if there is use this
	}
	return ret;
}

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned int	i;

	if (!dst && !src)
		return (NULL);
	if (dst > src)
	{
		while (len > 0)
		{
			((char *)dst)[len - 1] = ((char *)src)[len - 1];
			len--;
		}
	}
	else
	{
		i = 0;
		while (i < len)
		{
			((char *)dst)[i] = ((char *)src)[i];
			i++;
		}
	}
	return (dst);
}

void *realloc(void *ptr, size_t size)
{
    size_t user_size = size;
	t_block *metadata;
	t_block *new_block;

	if (!ptr) {
		return my_malloc(size);
	}
	if (get_block(ptr) == NULL) {
		return NULL;
	}
    if (size == 0) {
		my_free(ptr);
        return NULL;
	}
	metadata = (void *)ptr - BLOCK_SIZE;
	printf("realloc %ld new size %ld user_size %ld\n", metadata, size, metadata->user_size);
	if (size == metadata->user_size) {
		return ptr;
	}
	new_block = my_malloc(size);
	if (new_block == NULL) {
		return ptr;
	}
	ft_memmove(new_block, ptr, metadata->user_size > size ? size : metadata->user_size);	
	my_free(ptr);
	return new_block;
}

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
	// 	addr = (char*)my_malloc(1024);
	// 	addr[0] = 42;
	// 	my_free(addr);
	// 	i++;
	// }



	// addr1 = my_malloc(16 * M);
	// strcpy(addr1, "Bonjours\n");
	// print(addr1);
	// addr2 = my_malloc(16 * M);
	// addr3 = my_realloc(addr1, 128 * M);
	// addr3[127 * M] = 42;
	// print(addr3);

	// addr = my_malloc(16);
	// my_free(NULL);
	// my_free((void *)addr + 5);
	// if (my_realloc((void *)addr + 5, 10) == NULL)
	// 	print("Bonjours\n");


	// my_malloc(1024);
	// my_malloc(1024 * 32);
	// my_malloc(1024 * 1024);
	// my_malloc(1024 * 1024 * 16);
	// my_malloc(1024 * 1024 * 128);

	char *addr[180];

  addr[0] = my_malloc(20);
  addr[1] = my_malloc(40);
  addr[2] = my_malloc(4);
  addr[3] = my_malloc(15);
  addr[4] = my_malloc(42);
  addr[5] = my_malloc(89);
  addr[6] = my_malloc(589);
  addr[7] = my_malloc(400);
  addr[8] = my_malloc(123);
  show_alloc_mem();

  my_free(addr[0]);
  my_free(addr[3]);
  my_free(addr[5]);
  my_free(addr[7]);
  show_alloc_mem();

  addr[0] = my_malloc(42);
  addr[3] = my_malloc(2000);
  addr[5] = my_malloc(10000);
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
