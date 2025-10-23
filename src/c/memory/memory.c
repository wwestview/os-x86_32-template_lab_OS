#include "memory/memory.h"
#include "drivers/vga/vga.h"

static memory_manager_t memory_manager;

// Helper function to align size to 4-byte boundary
static u32 align_size(u32 size) {
    return (size + 3) & ~3;
}

// Helper function to get block from pointer
static memory_block_t* ptr_to_block(void* ptr) {
    return (memory_block_t*)((u8*)ptr - sizeof(memory_block_t));
}

// Helper function to get pointer from block
static void* block_to_ptr(memory_block_t* block) {
    return (void*)((u8*)block + sizeof(memory_block_t));
}

// Helper function to get block size (excluding header)
static u32 get_block_data_size(memory_block_t* block) {
    return block->size - sizeof(memory_block_t);
}

// Helper function to split block if it's too large
static void split_block(memory_block_t* block, u32 requested_size) {
    u32 total_size = requested_size + sizeof(memory_block_t);
    
    if (block->size > total_size + sizeof(memory_block_t)) {
        // Create new free block after the allocated block
        memory_block_t* new_block = (memory_block_t*)((u8*)block + total_size);
        new_block->size = block->size - total_size;
        new_block->is_free = true;
        new_block->next = block->next;
        new_block->prev = block;
        
        // Update block sizes
        block->size = total_size;
        
        // Update linked list
        if (block->next) {
            block->next->prev = new_block;
        }
        block->next = new_block;
        
        // Update memory manager
        memory_manager.block_count++;
        memory_manager.free_memory += new_block->size;
    }
}

// Helper function to merge adjacent free blocks
static void merge_free_blocks(memory_block_t* block) {
    // Merge with next block if it's free
    if (block->next && block->next->is_free) {
        memory_block_t* next_block = block->next;
        block->size += next_block->size;
        block->next = next_block->next;
        
        if (next_block->next) {
            next_block->next->prev = block;
        }
        
        memory_manager.block_count--;
        memory_manager.free_memory += sizeof(memory_block_t);
    }
    
    // Merge with previous block if it's free
    if (block->prev && block->prev->is_free) {
        memory_block_t* prev_block = block->prev;
        prev_block->size += block->size;
        prev_block->next = block->next;
        
        if (block->next) {
            block->next->prev = prev_block;
        }
        
        memory_manager.block_count--;
        memory_manager.free_memory += sizeof(memory_block_t);
    }
}

void memory_init(u32 heap_start_addr, u32 heap_size) {
    // Initialize memory manager
    memory_manager.heap_start = (memory_block_t*)heap_start_addr;
    memory_manager.heap_end = (memory_block_t*)(heap_start_addr + heap_size);
    memory_manager.total_heap_size = heap_size;
    memory_manager.free_memory = heap_size;
    memory_manager.allocated_memory = 0;
    memory_manager.block_count = 1;
    
    // Initialize the initial free block
    memory_block_t* initial_block = memory_manager.heap_start;
    initial_block->size = heap_size;
    initial_block->is_free = true;
    initial_block->next = 0;
    initial_block->prev = 0;
}

void* malloc(u32 size) {
    if (size == 0) {
        return 0;
    }
    
    // Align size to 4-byte boundary
    u32 aligned_size = align_size(size);
    u32 total_size = aligned_size + sizeof(memory_block_t);
    
    // Find the largest free block (worst fit)
    memory_block_t* worst_block = 0;
    memory_block_t* current = memory_manager.heap_start;
    
    while (current) {
        if (current->is_free && current->size >= total_size) {
            if (!worst_block || current->size > worst_block->size) {
                worst_block = current;
            }
        }
        current = current->next;
    }
    
    if (!worst_block) {
        // No suitable block found
        return 0;
    }
    
    // Mark block as allocated
    worst_block->is_free = false;
    
    // Update memory statistics
    memory_manager.allocated_memory += worst_block->size;
    memory_manager.free_memory -= worst_block->size;
    
    // Split block if it's too large
    split_block(worst_block, aligned_size);
    
    // Return pointer to data area
    return block_to_ptr(worst_block);
}

void free(void* ptr) {
    if (!ptr) {
        return;
    }
    
    // Get block from pointer
    memory_block_t* block = ptr_to_block(ptr);
    
    // Check if block is valid
    if (block < memory_manager.heap_start || block >= memory_manager.heap_end) {
        return; // Invalid pointer
    }
    
    if (block->is_free) {
        return; // Already free
    }
    
    // Mark block as free
    block->is_free = true;
    
    // Update memory statistics
    memory_manager.allocated_memory -= block->size;
    memory_manager.free_memory += block->size;
    
    // Merge with adjacent free blocks
    merge_free_blocks(block);
}

void memory_get_stats(u32* total, u32* free, u32* allocated, u32* blocks) {
    if (total) *total = memory_manager.total_heap_size;
    if (free) *free = memory_manager.free_memory;
    if (allocated) *allocated = memory_manager.allocated_memory;
    if (blocks) *blocks = memory_manager.block_count;
}

void memory_print_map() {
    vga_print_color("Memory Map:\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("Address    Size      Status\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("---------- --------- --------\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    memory_block_t* current = memory_manager.heap_start;
    u32 address = (u32)memory_manager.heap_start;
    
    while (current) {
        // Print address
        char addr_str[12];
        u32 temp_addr = address;
        u8 addr_len = 0;
        while (temp_addr > 0 && addr_len < 10) {
            addr_str[addr_len] = '0' + (temp_addr % 10);
            temp_addr /= 10;
            addr_len++;
        }
        addr_str[addr_len] = '\0';
        
        // Reverse address string
        for (u8 i = 0; i < addr_len / 2; i++) {
            char temp = addr_str[i];
            addr_str[i] = addr_str[addr_len - 1 - i];
            addr_str[addr_len - 1 - i] = temp;
        }
        
        vga_print_color(addr_str, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_print_color("  ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        // Print size
        char size_str[12];
        u32 temp_size = current->size;
        u8 size_len = 0;
        while (temp_size > 0 && size_len < 10) {
            size_str[size_len] = '0' + (temp_size % 10);
            temp_size /= 10;
            size_len++;
        }
        size_str[size_len] = '\0';
        
        // Reverse size string
        for (u8 i = 0; i < size_len / 2; i++) {
            char temp = size_str[i];
            size_str[i] = size_str[size_len - 1 - i];
            size_str[size_len - 1 - i] = temp;
        }
        
        vga_print_color(size_str, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_print_color("  ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        // Print status
        if (current->is_free) {
            vga_print_color("FREE", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        } else {
            vga_print_color("USED", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        }
        
        vga_newline();
        
        address += current->size;
        current = current->next;
    }
    
    vga_newline();
}

void memory_defragment() {
    // Simple defragmentation: merge all adjacent free blocks
    memory_block_t* current = memory_manager.heap_start;
    
    while (current) {
        if (current->is_free) {
            merge_free_blocks(current);
        }
        current = current->next;
    }
}

