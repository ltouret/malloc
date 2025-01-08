#ifndef MALLOC_H
# define MALLOC_H

#include <sys/mman.h>
#include <unistd.h>

#define ZONE_SIZE sizeof(t_zone)
#define BLOCK_SIZE sizeof(t_block)
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

#define TINY_SIZE (size_t) PAGE_SIZE / 4
#define SMALL_SIZE (size_t) PAGE_SIZE
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

extern t_zone *allocs;

void write_nb_base(unsigned int nb, unsigned int base);
size_t print_blocks(t_block *block);
void show_alloc_mem();
size_t get_zone_size(e_zone type, size_t large_alloc);
e_zone get_zone_type(size_t size);
void *get_zone_of_block(void *ptr, size_t type_zone_size);
void *get_block(t_block *ptr);
int count_zone_type(e_zone type);
void remove_zone(t_zone *zone);
size_t align_memory(size_t size);
void free(void *ptr);
void *ft_memmove(void *dst, const void *src, size_t len);
void *realloc(void *ptr, size_t size);
t_block *create_block(t_block *ptr, t_block *prev, t_block *next, size_t size,  size_t user_size, size_t free);
void add_zone(t_zone *zone);
void *create_zone(size_t size, e_zone zone_type);
void *create_large(size_t size, size_t user_size);
void *create_tiny_small(size_t size, size_t user_size);
void *init();
void *malloc(size_t size);

#endif