#ifndef MEMORY_H
#define MEMORY_H

#include "kernel/kernel.h"

// Memory block structure
typedef struct memory_block {
    u32 size;                    // Size of the block (including header)
    bool is_free;                // True if block is free, false if allocated
    struct memory_block* next;   // Pointer to next block
    struct memory_block* prev;   // Pointer to previous block
} memory_block_t;

// Memory manager structure
typedef struct {
    memory_block_t* heap_start;  // Start of heap
    memory_block_t* heap_end;    // End of heap
    u32 total_heap_size;         // Total heap size
    u32 free_memory;             // Total free memory
    u32 allocated_memory;        // Total allocated memory
    u32 block_count;             // Number of blocks
} memory_manager_t;

// Initialize memory manager
void memory_init(u32 heap_start_addr, u32 heap_size);

// Allocate memory using worst fit algorithm
void* malloc(u32 size);

// Free allocated memory
void free(void* ptr);

// Get memory statistics
void memory_get_stats(u32* total, u32* free, u32* allocated, u32* blocks);

// Print memory map (for debugging)
void memory_print_map();

// Defragment memory
void memory_defragment();

#endif

