#include <sys/mman.h>
#include <unistd.h>

#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

#define TINY_SIZE PAGE_SIZE / 4
#define SMALL_SIZE PAGE_SIZE
#define ALLOCATIONS 105
#define TINY_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + TINY_SIZE) + ZONE_SIZE) / PAGE_SIZE) + 1) * PAGE_SIZE
#define SMALL_ZONE_SIZE (((ALLOCATIONS * (BLOCK_SIZE + SMALL_SIZE) + ZONE_SIZE) / PAGE_SIZE) + 1) * PAGE_SIZE

#define FREE 1
#define NOTFREE 0

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
	unsigned int size;
	unsigned int free;
} t_block;

typedef struct s_zone
{
	struct s_zone *next;
	t_block *block;
	size_t free_space;
	e_zone type;
	int padding;
} t_zone;

t_zone *allocs;

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
	t_zone *zone = allocs;
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

size_t get_zone_size(e_zone type, size_t large_alloc)
{
	size_t size = 0;
	if (type == TYPE_TINY) {
		size = TINY_ZONE_SIZE;
	} else if (type == TYPE_SMALL) {
		size = SMALL_ZONE_SIZE;
	} else {
  		size = (large_alloc + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	}
	return size;
}

e_zone get_zone_type(size_t size)
{
	return (0 * (size <= TINY_SIZE)) +
		   (1 * ((size > TINY_SIZE) && (size <= SMALL_SIZE))) +
		   (2 * (size > SMALL_SIZE));
}

void *get_zone_of_block(void *ptr, size_t type_zone_size) {
	t_zone *curr_zone = allocs;

	while (curr_zone)
	{
		if ((void *)curr_zone <= ptr && (void *)curr_zone + type_zone_size > ptr) {
			break;
		}
		curr_zone = curr_zone->next;
	}
	return curr_zone;
}

void *get_block(t_block *ptr)
{
	t_zone *curr_zone = allocs;
	while (curr_zone) {
		t_block *curr_block = curr_zone->block;
		while (curr_block) {
			if ((void *)ptr == (void *)curr_block + BLOCK_SIZE) {
				return ptr;
			}
			curr_block = curr_block->next;
		}
		curr_zone = curr_zone->next;
	}
	return NULL;
}

int count_zone_type(e_zone type)
{
	t_zone *curr_zone = allocs;
	int count = 0;

	while (curr_zone) {
		if (curr_zone->type == type) {
			count++;
		}
		curr_zone = curr_zone->next;
	}
	return count;
}

void remove_zone(t_zone *zone)
{
	t_zone *curr_zone = allocs;
	t_zone *prev = NULL;

	if (curr_zone == zone) {
		allocs = zone->next;
		return;
	} 
	while (curr_zone) {
		if (curr_zone == zone) {
			prev->next = curr_zone->next;
			break;
		}
		prev = curr_zone;
		curr_zone = curr_zone->next;
	}
}

void my_free(void *ptr)
{
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
	e_zone zone_type = get_zone_type(metadata->size);
	size_t zone_size = get_zone_size(zone_type, 0);
	t_zone *zone = get_zone_of_block(ptr, zone_size);
	if (zone_type < TYPE_LARGE) {
		metadata->free = FREE;
		zone->free_space += (metadata->size + BLOCK_SIZE);
		int count_zone = count_zone_type(zone_type);
		if (count_zone > 1 && zone->free_space + ZONE_SIZE == zone_size) {
			remove_zone(zone);
			munmap((void *)zone, zone->free_space + ZONE_SIZE);
		}
	} else {
		zone = (void *)ptr - BLOCK_SIZE - ZONE_SIZE;
		remove_zone(zone);
		munmap((void *)zone, zone->free_space + ZONE_SIZE);
  	}	
}

size_t align_memory(size_t size)
{
	return (size + 15) & ~15;
}

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
	t_zone *curr_zone = allocs;
	if (curr_zone == NULL) {
		allocs = zone;
		return;
	}
	while (curr_zone->next) {
		curr_zone = curr_zone->next;
	}
	curr_zone->next = zone;
}

void *create_zone(size_t size, e_zone zone_type)
{
	size_t zone_size = get_zone_size(zone_type, size);
	t_zone *zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED)	{
		return NULL;
	}
	zone->next = NULL;
	zone->block = NULL;
	zone->type = zone_type;
	zone->free_space = zone_size - ZONE_SIZE;
	add_zone(zone);
	return zone;
}

void *create_large(t_zone **zone, size_t size, size_t user_size)
{
	t_zone *curr_zone = create_zone(size + BLOCK_SIZE, TYPE_LARGE);
	t_block *ptr = create_block((void *)curr_zone + ZONE_SIZE, NULL, NULL, size, user_size, NOTFREE);
	curr_zone->block = ptr;
	return (void *)ptr + BLOCK_SIZE;
}

void *create_tiny_small(t_zone **zone, size_t size, size_t user_size)
{
	t_zone *curr_zone = *zone;
	size_t zone_type = get_zone_type(size);
	while (curr_zone) {
		if (curr_zone->type == zone_type && BLOCK_SIZE + size < curr_zone->free_space) {
			break;
		}
		curr_zone = curr_zone->next;
	}
	if (!curr_zone) {
		curr_zone = create_zone(size, zone_type);
	}
	if (curr_zone && curr_zone->type == zone_type && size + BLOCK_SIZE <= curr_zone->free_space)
	{
		t_block *curr_block = curr_zone->block;
		if (!curr_block) {
			t_block *ptr = create_block((void *)curr_zone + ZONE_SIZE, NULL, NULL, size, user_size, NOTFREE);
			curr_zone->block = ptr;
			curr_zone->free_space -= (size + BLOCK_SIZE);
			return (void *)ptr + BLOCK_SIZE;
		}
		while (curr_block->next) {
			//? maybe can add this condition in the loop directly
			if (curr_block->free == FREE && size <= curr_block->size) {
				break;
			}
			curr_block = curr_block->next;
		}
		if (curr_block->free == FREE && size <= curr_block->size) {
			curr_block->free = NOTFREE;
			curr_block->user_size = user_size;
			curr_zone->free_space -= (curr_block->size + BLOCK_SIZE);
			return (void *)curr_block + BLOCK_SIZE;
		}
		else {
			t_block *ptr = create_block((void *)curr_block + curr_block->size + BLOCK_SIZE, curr_block, NULL, size, user_size, NOTFREE);
			curr_block->next = ptr;
			curr_zone->free_space -= (size + BLOCK_SIZE);
			return (void *)ptr + BLOCK_SIZE;
		}
	}
	return NULL;
}

void *init()
{
	if (allocs) {
		return allocs;
	}

	t_zone *tiny = create_zone(TINY_ZONE_SIZE, TYPE_TINY);
	t_zone *tiny2 = create_zone(TINY_ZONE_SIZE, TYPE_TINY);
	if (tiny == MAP_FAILED || tiny2 == MAP_FAILED) {
		return NULL;
	}

	t_zone *small = create_zone(SMALL_ZONE_SIZE, TYPE_SMALL);
	t_zone *small2 = create_zone(SMALL_ZONE_SIZE, TYPE_SMALL);
	if (small == MAP_FAILED || small2 == MAP_FAILED) {
		return NULL;
	}
	return allocs;
}

void *my_malloc(size_t size)
{
	void *ret = NULL;
	size_t user_size = size;

	if (size == 0) {
		return NULL;
	}
	if (init() == NULL) {
		return NULL;
	}
	if (size > SMALL_SIZE) {
		ret = create_large(&allocs, size, user_size);
	}
	else {
		ret = create_tiny_small(&allocs, size, user_size);
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

//! move this to main test

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
