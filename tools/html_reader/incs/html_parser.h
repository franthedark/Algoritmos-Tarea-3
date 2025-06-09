#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Función para strcasestr si no está disponible
#ifndef strcasestr
char* strcasestr(const char* haystack, const char* needle);
#endif

// Constantes
#define MAX_LINE_LENGTH 4096
#define MAX_FILENAME_LENGTH 256

// Estructuras
typedef struct {
    char* content;
    size_t size;
    size_t capacity;
} TextBuffer;

// Funciones principales
TextBuffer* create_buffer(size_t initial_capacity);
void destroy_buffer(TextBuffer* buffer);
bool append_to_buffer(TextBuffer* buffer, const char* text);
char* read_file(const char* filename);
char* strip_html_tags(const char* html_content);
void save_text_file(const char* filename, const char* content);
void print_usage(const char* program_name);

// Funciones auxiliares
bool is_html_tag_start(const char* str, size_t pos);
bool is_omittable_tag(const char* str, size_t pos);
size_t skip_omittable_content(const char* str, size_t pos);
size_t skip_html_tag(const char* str, size_t pos);
void decode_html_entities(char* text);
void normalize_whitespace(char* text);
void trim_whitespace(char* text);
bool should_add_newline(const char* str, size_t pos);

#endif