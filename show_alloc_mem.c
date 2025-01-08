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