# Custom Memory Allocator

## Overview
This project implements a custom memory allocator for **Linux**, similar to `malloc`, `free`, and `realloc` in C.The implementation manages memory allocation efficiently while providing memory visualization utilities.

## Installation & Compilation
To compile the project, use the provided `Makefile`:

```sh
make
```

This will generate a shared library (`libft_malloc.so`).

## Usage
### Running the allocator
Since this project is a shared library, you need to preload it to override the default `malloc` functions. Use the provided `run.sh` script:

```sh
./run.sh your_program
```

This script sets up the necessary environment variables (`LD_LIBRARY_PATH` and `LD_PRELOAD`) and launches `your_program` with the custom memory allocator.

## Implementation Details
- **Memory Allocation**: Uses `mmap` and `munmap` for memory management.
- **Data Structure**: Implements a **doubly linked list** to track memory blocks.
- **First-Fit Allocation**: When memory is freed and reallocated, a **first-fit** strategy is used to reuse available space efficiently.
- **Memory Alignment**: All allocations are **aligned to 16 bytes** to ensure proper memory access and performance.
- **Preallocated Zones**: To optimize performance, **two tiny zones and two small zones** are preallocated, reducing frequent system calls.
  - `TINY_SIZE`: Defined as **PAGE_SIZE / 4** (typically **1024 bytes** per allocation).
  - `SMALL_SIZE`: Defined as **PAGE_SIZE** (typically **4096 bytes** per allocation).
  - **Tiny Zone Size**: Preallocated in multiples of `PAGE_SIZE` to hold small allocations efficiently.
  - **Small Zone Size**: Similarly preallocated to reduce the overhead of frequent system calls.
- **Memory Visualization**: The `show_alloc_mem()` file provides a function to display the current state of allocated memory. It helps keep track of memory usage by listing allocated blocks and their sizes.

## License
This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.  
You are free to use, modify, and redistribute it under the same license.

For the full text of the GPLv3, visit:  
[https://www.gnu.org/licenses/gpl-3.0.html](https://www.gnu.org/licenses/gpl-3.0.html).

## Contributors
- LT
- Fdeĉ