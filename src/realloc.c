#include "malloc.h"

void *ft_memmove(void *dst, const void *src, size_t len)
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
	t_block *metadata;
	t_block *new_block;

	if (!ptr) {
		return malloc(size);
	}
	if (get_block(ptr) == NULL) {
		return NULL;
	}
    if (size == 0) {
		free(ptr);
        return NULL;
	}
	metadata = (void *)ptr - BLOCK_SIZE;
	if (size == metadata->user_size) {
		return ptr;
	}
	new_block = malloc(size);
	if (new_block == NULL) {
		return ptr;
	}
	ft_memmove(new_block, ptr, metadata->user_size > size ? size : metadata->user_size);	
	free(ptr);
	return new_block;
}