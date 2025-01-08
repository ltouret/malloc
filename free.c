#include "malloc.h"

void free(void *ptr)
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