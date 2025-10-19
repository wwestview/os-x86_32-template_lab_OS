#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "kernel/kernel.h"

#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_SIZE 2000
#define MAX_FILE_CONTENT_LINES 25
#define MAX_LINE_LENGTH 80

// File structure
typedef struct {
    char name[MAX_FILENAME_LENGTH];
    char content[MAX_FILE_SIZE];
    u16 content_length;
    bool exists;
} file_t;

// File system structure
typedef struct {
    file_t files[MAX_FILES];
    u8 file_count;
} filesystem_t;

// Initialize file system
void fs_init();

// Create a new file
bool fs_create_file(const char* filename);

// Delete a file
bool fs_delete_file(const char* filename);

// Check if file exists
bool fs_file_exists(const char* filename);

// Get file by name
file_t* fs_get_file(const char* filename);

// List all files
void fs_list_files();

// Write content to file
bool fs_write_file(const char* filename, const char* content);

// Read file content
bool fs_read_file(const char* filename, char* buffer, u16 buffer_size);

// Append content to file
bool fs_append_file(const char* filename, const char* content);

// Insert content at position
bool fs_insert_at_position(const char* filename, const char* content, u16 position);

// Delete content from position
bool fs_delete_from_position(const char* filename, u16 position, u16 length);

// Replace content in file
bool fs_replace_content(const char* filename, const char* old_text, const char* new_text);

// Get file size
u16 fs_get_file_size(const char* filename);

// Clear file content
bool fs_clear_file(const char* filename);

// Get file system instance
filesystem_t* fs_get_instance();

#endif
