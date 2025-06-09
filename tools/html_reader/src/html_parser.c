#include "html_parser.h"

// Crear un buffer de texto dinámico
TextBuffer* create_buffer(size_t initial_capacity) {
    TextBuffer* buffer = malloc(sizeof(TextBuffer));
    if (!buffer) return NULL;
    
    buffer->content = malloc(initial_capacity);
    if (!buffer->content) {
        free(buffer);
        return NULL;
    }
    
    buffer->content[0] = '\0';
    buffer->size = 0;
    buffer->capacity = initial_capacity;
    
    return buffer;
}

// Destruir buffer
void destroy_buffer(TextBuffer* buffer) {
    if (buffer) {
        free(buffer->content);
        free(buffer);
    }
}

// Agregar texto al buffer (redimensiona si es necesario)
bool append_to_buffer(TextBuffer* buffer, const char* text) {
    if (!buffer || !text) return false;
    
    size_t text_len = strlen(text);
    size_t needed_capacity = buffer->size + text_len + 1;
    
    if (needed_capacity > buffer->capacity) {
        size_t new_capacity = buffer->capacity * 2;
        if (new_capacity < needed_capacity) {
            new_capacity = needed_capacity;
        }
        
        char* new_content = realloc(buffer->content, new_capacity);
        if (!new_content) return false;
        
        buffer->content = new_content;
        buffer->capacity = new_capacity;
    }
    
    strcat(buffer->content, text);
    buffer->size += text_len;
    
    return true;
}

// Leer archivo completo
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: No se puede abrir el archivo '%s'\n", filename);
        return NULL;
    }
    
    // Obtener tamaño del archivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < 0) {
        fclose(file);
        return NULL;
    }
    
    // Reservar memoria
    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    // Leer archivo
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';
    
    fclose(file);
    return content;
}

// Verificar si la posición actual es el inicio de una etiqueta HTML
bool is_html_tag_start(const char* str, size_t pos) {
    if (str[pos] == '<') {
        // Verificar si es un comentario (<!--)
        if (str[pos+1] == '!' && str[pos+2] == '-' && str[pos+3] == '-') {
            return true;
        }
        return true;
    }
    return false;
}

// Saltar una etiqueta HTML completa
// Saltar una etiqueta HTML completa (incluyendo contenido de style/script)
size_t skip_html_tag(const char* str, size_t pos) {
    if (str[pos] != '<') return pos;
    
    size_t i = pos + 1;
    bool in_quotes = false;
    char quote_char = '\0';
    bool is_style_or_script = false;
    char tag_name[10] = {0};
    size_t tag_name_len = 0;
    
    // Extraer el nombre de la etiqueta
    while (i < pos + 10 && str[i] && !isspace(str[i]) && str[i] != '>') {
        tag_name[tag_name_len++] = tolower(str[i]);
        i++;
    }
    tag_name[tag_name_len] = '\0';
    
    // Verificar si es style o script
    is_style_or_script = (strcmp(tag_name, "style") == 0) || (strcmp(tag_name, "script") == 0);
    
    // Buscar el cierre de la etiqueta de apertura
    while (str[i] != '\0') {
        if (!in_quotes && (str[i] == '"' || str[i] == '\'')) {
            in_quotes = true;
            quote_char = str[i];
        } else if (in_quotes && str[i] == quote_char) {
            in_quotes = false;
        } else if (!in_quotes && str[i] == '>') {
            i++; // Avanzar más allá del '>'
            break;
        }
        i++;
    }
    
    // Si es style o script, buscar la etiqueta de cierre correspondiente
    if (is_style_or_script) {
        char close_tag[20];
        snprintf(close_tag, sizeof(close_tag), "</%s>", tag_name);
        size_t close_tag_len = strlen(close_tag);
        
        while (str[i] != '\0') {
            if (strncmp(&str[i], close_tag, close_tag_len) == 0) {
                return i + close_tag_len; // Posición después de la etiqueta de cierre
            }
            i++;
        }
    }
    
    return i; // Si no se encuentra '>', devolver final de cadena
}

// Decodificar entidades HTML básicas
void decode_html_entities(char* text) {
    char* src = text;
    char* dst = text;
    
    while (*src) {
        if (*src == '&') {
            if (strncmp(src, "&amp;", 5) == 0) {
                *dst++ = '&';
                src += 5;
            } else if (strncmp(src, "&lt;", 4) == 0) {
                *dst++ = '<';
                src += 4;
            } else if (strncmp(src, "&gt;", 4) == 0) {
                *dst++ = '>';
                src += 4;
            } else if (strncmp(src, "&quot;", 6) == 0) {
                *dst++ = '"';
                src += 6;
            } else if (strncmp(src, "&apos;", 6) == 0) {
                *dst++ = '\'';
                src += 6;
            } else if (strncmp(src, "&nbsp;", 6) == 0) {
                *dst++ = ' ';
                src += 6;
            } else {
                *dst++ = *src++;
            }
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

// Eliminar espacios en blanco al inicio y final
void trim_whitespace(char* text) {
    if (!text) return;
    
    // Eliminar espacios al inicio
    char* start = text;
    while (isspace(*start)) start++;
    
    // Eliminar espacios al final
    char* end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    
    // Mover contenido
    size_t len = end - start + 1;
    memmove(text, start, len);
    text[len] = '\0';
}

// Función principal: quitar etiquetas HTML
char* strip_html_tags(const char* html_content) {
    if (!html_content) return NULL;
    
    TextBuffer* buffer = create_buffer(strlen(html_content) + 1);
    if (!buffer) return NULL;
    
    size_t len = strlen(html_content);
    size_t i = 0;
    
    while (i < len) {
        if (is_html_tag_start(html_content, i)) {
            // Saltar la etiqueta HTML
            i = skip_html_tag(html_content, i);
        } else {
            // Agregar carácter al buffer
            char temp[2] = {html_content[i], '\0'};
            append_to_buffer(buffer, temp);
            i++;
        }
    }
    
    // Decodificar entidades HTML
    decode_html_entities(buffer->content);
    
    // Limpiar espacios excesivos
    trim_whitespace(buffer->content);
    
    // Crear copia del resultado
    char* result = malloc(strlen(buffer->content) + 1);
    if (result) {
        strcpy(result, buffer->content);
    }
    
    destroy_buffer(buffer);
    return result;
}

// Guardar contenido en archivo de texto
void save_text_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: No se puede crear el archivo '%s'\n", filename);
        return;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
    printf("Texto guardado en: %s\n", filename);
}

// Mostrar instrucciones de uso
void print_usage(const char* program_name) {
    printf("Uso: %s <archivo_html> [archivo_salida]\n", program_name);
    printf("  archivo_html   : Archivo HTML de entrada\n");
    printf("  archivo_salida : Archivo de texto de salida (opcional)\n");
    printf("\nEjemplo:\n");
    printf("  %s documento.html\n", program_name);
    printf("  %s documento.html texto_limpio.txt\n", program_name);
}