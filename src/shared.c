#include "malloc.h"

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

void *get_zone_of_block(void *ptr, size_t type_zone_size)
{
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

size_t align_memory(size_t size)
{
	return (size + 15) & ~15;
}
