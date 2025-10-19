#include "filesystem/filesystem.h"
#include "drivers/vga/vga.h"

static filesystem_t filesystem;

// Helper function to compare strings
static bool str_equals(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) return false;
        str1++;
        str2++;
    }
    return *str1 == *str2;
}

// Helper function to copy string
static void str_copy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// Helper function to get string length
static u16 str_length(const char* str) {
    u16 len = 0;
    while (*str) {
        len++;
        str++;
    }
    return len;
}

// Helper function to convert string to number
static u16 str_to_number(const char* str) {
    u16 result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

void fs_init() {
    filesystem.file_count = 0;
    
    // Initialize all files as non-existent
    for (u8 i = 0; i < MAX_FILES; i++) {
        filesystem.files[i].exists = false;
        filesystem.files[i].name[0] = '\0';
        filesystem.files[i].content[0] = '\0';
        filesystem.files[i].content_length = 0;
    }
}

bool fs_create_file(const char* filename) {
    if (filesystem.file_count >= MAX_FILES) {
        return false; // No space for new files
    }
    
    // Check if file already exists
    if (fs_file_exists(filename)) {
        return false;
    }
    
    // Find empty slot
    for (u8 i = 0; i < MAX_FILES; i++) {
        if (!filesystem.files[i].exists) {
            str_copy(filesystem.files[i].name, filename);
            filesystem.files[i].exists = true;
            filesystem.files[i].content[0] = '\0';
            filesystem.files[i].content_length = 0;
            filesystem.file_count++;
            return true;
        }
    }
    
    return false;
}

bool fs_delete_file(const char* filename) {
    for (u8 i = 0; i < MAX_FILES; i++) {
        if (filesystem.files[i].exists && str_equals(filesystem.files[i].name, filename)) {
            filesystem.files[i].exists = false;
            filesystem.files[i].name[0] = '\0';
            filesystem.files[i].content[0] = '\0';
            filesystem.files[i].content_length = 0;
            filesystem.file_count--;
            return true;
        }
    }
    return false;
}

bool fs_file_exists(const char* filename) {
    for (u8 i = 0; i < MAX_FILES; i++) {
        if (filesystem.files[i].exists && str_equals(filesystem.files[i].name, filename)) {
            return true;
        }
    }
    return false;
}

file_t* fs_get_file(const char* filename) {
    for (u8 i = 0; i < MAX_FILES; i++) {
        if (filesystem.files[i].exists && str_equals(filesystem.files[i].name, filename)) {
            return &filesystem.files[i];
        }
    }
    return 0;
}

void fs_list_files() {
    vga_newline();
    vga_print_color("Files in memory:\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("===============\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    if (filesystem.file_count == 0) {
        vga_print_color("No files found.\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        return;
    }
    
    for (u8 i = 0; i < MAX_FILES; i++) {
        if (filesystem.files[i].exists) {
            vga_print_color("  ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            vga_print(filesystem.files[i].name);
            vga_print_color(" (", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            // Print file size
            char size_str[8];
            u16 size = filesystem.files[i].content_length;
            u8 pos = 0;
            if (size == 0) {
                size_str[pos++] = '0';
            } else {
                while (size > 0) {
                    size_str[pos++] = '0' + (size % 10);
                    size /= 10;
                }
            }
            size_str[pos] = '\0';
            // Reverse the string
            for (u8 j = 0; j < pos / 2; j++) {
                char temp = size_str[j];
                size_str[j] = size_str[pos - 1 - j];
                size_str[pos - 1 - j] = temp;
            }
            vga_print(size_str);
            vga_print_color(" chars)\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    vga_newline();
}

bool fs_write_file(const char* filename, const char* content) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    u16 content_len = str_length(content);
    if (content_len >= MAX_FILE_SIZE) {
        return false; // Content too large
    }
    
    str_copy(file->content, content);
    file->content_length = content_len;
    return true;
}

bool fs_read_file(const char* filename, char* buffer, u16 buffer_size) {
    file_t* file = fs_get_file(filename);
    if (!file || !buffer) {
        return false;
    }
    
    u16 copy_len = file->content_length;
    if (copy_len >= buffer_size) {
        copy_len = buffer_size - 1;
    }
    
    for (u16 i = 0; i < copy_len; i++) {
        buffer[i] = file->content[i];
    }
    buffer[copy_len] = '\0';
    return true;
}

bool fs_append_file(const char* filename, const char* content) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    u16 content_len = str_length(content);
    u16 new_length = file->content_length + content_len;
    
    if (new_length >= MAX_FILE_SIZE) {
        return false; // Would exceed max size
    }
    
    // Append content
    for (u16 i = 0; i < content_len; i++) {
        file->content[file->content_length + i] = content[i];
    }
    file->content[new_length] = '\0';
    file->content_length = new_length;
    return true;
}

bool fs_insert_at_position(const char* filename, const char* content, u16 position) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    u16 content_len = str_length(content);
    u16 new_length = file->content_length + content_len;
    
    if (position > file->content_length || new_length >= MAX_FILE_SIZE) {
        return false;
    }
    
    // Shift existing content right
    for (u16 i = file->content_length; i > position; i--) {
        file->content[i + content_len - 1] = file->content[i - 1];
    }
    
    // Insert new content
    for (u16 i = 0; i < content_len; i++) {
        file->content[position + i] = content[i];
    }
    
    file->content[new_length] = '\0';
    file->content_length = new_length;
    return true;
}

bool fs_delete_from_position(const char* filename, u16 position, u16 length) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    if (position >= file->content_length) {
        return false;
    }
    
    if (position + length > file->content_length) {
        length = file->content_length - position;
    }
    
    // Shift content left
    for (u16 i = position; i < file->content_length - length; i++) {
        file->content[i] = file->content[i + length];
    }
    
    file->content_length -= length;
    file->content[file->content_length] = '\0';
    return true;
}

bool fs_replace_content(const char* filename, const char* old_text, const char* new_text) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    u16 old_len = str_length(old_text);
    u16 new_len = str_length(new_text);
    
    // Find first occurrence of old_text
    for (u16 i = 0; i <= file->content_length - old_len; i++) {
        bool found = true;
        for (u16 j = 0; j < old_len; j++) {
            if (file->content[i + j] != old_text[j]) {
                found = false;
                break;
            }
        }
        
        if (found) {
            // Calculate new length
            u16 new_content_length = file->content_length - old_len + new_len;
            if (new_content_length >= MAX_FILE_SIZE) {
                return false;
            }
            
            // Shift content if new text is longer
            if (new_len > old_len) {
                for (u16 k = file->content_length; k > i + old_len; k--) {
                    file->content[k + new_len - old_len - 1] = file->content[k - 1];
                }
            }
            // Shift content if new text is shorter
            else if (new_len < old_len) {
                for (u16 k = i + old_len; k < file->content_length; k++) {
                    file->content[k - old_len + new_len] = file->content[k];
                }
            }
            
            // Insert new text
            for (u16 j = 0; j < new_len; j++) {
                file->content[i + j] = new_text[j];
            }
            
            file->content_length = new_content_length;
            file->content[file->content_length] = '\0';
            return true;
        }
    }
    
    return false; // old_text not found
}

u16 fs_get_file_size(const char* filename) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return 0;
    }
    return file->content_length;
}

bool fs_clear_file(const char* filename) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    file->content[0] = '\0';
    file->content_length = 0;
    return true;
}

filesystem_t* fs_get_instance() {
    return &filesystem;
}