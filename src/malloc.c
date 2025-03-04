#include "malloc.h"

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

void *create_large(size_t size, size_t user_size)
{
	t_zone *curr_zone = create_zone(size + BLOCK_SIZE, TYPE_LARGE);
	t_block *ptr = create_block((void *)curr_zone + ZONE_SIZE, NULL, NULL, size, user_size, NOTFREE);
	curr_zone->block = ptr;
	return (void *)ptr + BLOCK_SIZE;
}

void *create_tiny_small(size_t size, size_t user_size)
{
	t_zone *curr_zone = allocs;
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

t_zone *allocs = NULL;

void *malloc(size_t size)
{
	void *ret = NULL;
	size_t user_size = size;
	
	if (size == 0) {
		return NULL;
	}
	if (init() == NULL) {
		return NULL;
	}

	size = align_memory(size);
	if (size > SMALL_SIZE) {
		ret = create_large(size, user_size);
	}
	else {
		ret = create_tiny_small(size, user_size);
	}
	return ret;
}