#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <string.h>
#include "libft.h"

//135168 first malloc

// tiny malloc = 28762
// 111 mallocs of 256 (real 240)

typedef struct	s_heap
{
	struct s_heap	*next;
	int				free_size;
	//char			align[4]; // dis needed?
	int				begin;
}	t_heap;

typedef struct	s_alloc
{
	t_heap		*tiny;
	t_heap		*small;
	t_heap		*large;
}	t_alloc;

t_alloc	alloc_list;

int		go_next_block(int size)
{
	//same with bitwise
	//(10 + 15) & ~15;
	return ((size + 15) - ((size + 15) % 16));
}

int		add_padding(int size)
{
	return ((size + 15) - ((size + 15) % 16)) - size;
}

//int		get_type(int type)
//{
//	if ()
//}

long	create_block(int size);

void	*create_tiny(void) // create enum with type?
{
	if (alloc_list.tiny == NULL)
	{
		alloc_list.tiny = mmap(NULL, 28672, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		printf("malloc tiny %p\n", alloc_list.tiny);
		ft_bzero(alloc_list.tiny, 28672);
		alloc_list.tiny.free_size = 28672 - 36; // 16 heap - 8 padd - 8 block - 4 end
		// maybe change end to 8 bytes?
		// no need to add end to 0 as its already done with bzero 
		// b4 ret need to create first block 16 8 (8 28636) 4
		return alloc_list.tiny + 16 + 8;
	}
	return NULL;
}

void	*my_malloc(size_t size)
{
	if (size == 0)
		return NULL;
	if (size <= 240)
	{
		return (create_tiny());
	}
	// if SMALL
	// if LARGE
	return NULL;
}
void bin(unsigned n)
{
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2)
	{
        (n & i) ? printf("1") : printf("0");
	}
	printf("\n");
}

int main()
{
	my_malloc(10);
	my_malloc(10);
	printf("%ld\n", sizeof(t_heap));
	struct rlimit rpl;
	(getrlimit(RLIMIT_DATA, &rpl));
	printf("%d %ld\n", (10 + 15) & ~15, rpl.rlim_cur);
	int i = 5;
	while (i--)
	{
		char *ptr = malloc(27);
		double res = (unsigned long) ptr / 4096.0;
		printf("malloc %p %lu %f\n", ptr, (unsigned long) ptr , res);
		//free(ptr);
	}
	i = 5;
	while (i--)
	{
		char *ptr = mmap(NULL, 31, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		double res = (unsigned long) ptr / 4096.0;
		printf("mmap %p %lu %f\n", ptr, (unsigned long) ptr , res);
		//free(ptr);
		munmap(ptr,35);
	}
	return 0;
}
