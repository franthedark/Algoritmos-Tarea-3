#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estructura para mapeos Unicode case-folding
typedef struct {
    const char* from;
    const char* to;
} UnicodeMapping;

// Tabla extendida de case-folding Unicode
static const UnicodeMapping case_folding_table[] = {
    // Latín básico extendido
    {"A", "a"}, {"B", "b"}, {"C", "c"}, {"D", "d"}, {"E", "e"}, {"F", "f"}, 
    {"G", "g"}, {"H", "h"}, {"I", "i"}, {"J", "j"}, {"K", "k"}, {"L", "l"},
    {"M", "m"}, {"N", "n"}, {"O", "o"}, {"P", "p"}, {"Q", "q"}, {"R", "r"},
    {"S", "s"}, {"T", "t"}, {"U", "u"}, {"V", "v"}, {"W", "w"}, {"X", "x"},
    {"Y", "y"}, {"Z", "z"},
    
    // Casos especiales Unicode
    {"İ", "i̇"},  // I turca con punto
    {"I", "ı"},   // I sin punto -> ı (en contexto turco)
    {"ß", "ss"},  // Eszett alemana
    {"ſ", "s"},   // S larga
    {"ﬀ", "ff"}, {"ﬁ", "fi"}, {"ﬂ", "fl"}, {"ﬃ", "ffi"}, {"ﬄ", "ffl"}, // Ligaduras
    
    // Letras con diacríticos - Español/Latinoamérica
    {"Á", "á"}, {"À", "à"}, {"Â", "â"}, {"Ä", "ä"}, {"Ã", "ã"}, {"Å", "å"},
    {"É", "é"}, {"È", "è"}, {"Ê", "ê"}, {"Ë", "ë"}, 
    {"Í", "í"}, {"Ì", "ì"}, {"Î", "î"}, {"Ï", "ï"},
    {"Ó", "ó"}, {"Ò", "ò"}, {"Ô", "ô"}, {"Ö", "ö"}, {"Õ", "õ"}, {"Ø", "ø"},
    {"Ú", "ú"}, {"Ù", "ù"}, {"Û", "û"}, {"Ü", "ü"},
    {"Ñ", "ñ"}, {"Ç", "ç"},
    
    // Caracteres adicionales para países de habla inglesa
    {"Æ", "æ"}, {"Œ", "œ"}, {"Þ", "þ"}, {"Ð", "ð"},
    
    // Cirílico básico
    {"А", "а"}, {"Б", "б"}, {"В", "в"}, {"Г", "г"}, {"Д", "д"}, {"Е", "е"},
    {"Ё", "ё"}, {"Ж", "ж"}, {"З", "з"}, {"И", "и"}, {"Й", "й"}, {"К", "к"},
    {"Л", "л"}, {"М", "м"}, {"Н", "н"}, {"О", "о"}, {"П", "п"}, {"Р", "р"},
    {"С", "с"}, {"Т", "т"}, {"У", "у"}, {"Ф", "ф"}, {"Х", "х"}, {"Ц", "ц"},
    {"Ч", "ч"}, {"Ш", "ш"}, {"Щ", "щ"}, {"Ъ", "ъ"}, {"Ы", "ы"}, {"Ь", "ь"},
    {"Э", "э"}, {"Ю", "ю"}, {"Я", "я"},
    
    {NULL, NULL} // Terminador
};

// Tabla de normalización canónica (NFD -> NFC)
static const UnicodeMapping canonical_normalization[] = {
    // Formas descompuestas -> precompuestas
    {"a\xCC\x81", "á"}, {"a\xCC\x80", "à"}, {"a\xCC\x82", "â"}, {"a\xCC\x84", "ā"},
    {"a\xCC\x88", "ä"}, {"a\xCC\x83", "ã"}, {"a\xCC\x8A", "å"},
    {"e\xCC\x81", "é"}, {"e\xCC\x80", "è"}, {"e\xCC\x82", "ê"}, {"e\xCC\x88", "ë"},
    {"i\xCC\x81", "í"}, {"i\xCC\x80", "ì"}, {"i\xCC\x82", "î"}, {"i\xCC\x88", "ï"},
    {"o\xCC\x81", "ó"}, {"o\xCC\x80", "ò"}, {"o\xCC\x82", "ô"}, {"o\xCC\x88", "ö"},
    {"o\xCC\x83", "õ"}, {"o\xCC\x84", "ō"},
    {"u\xCC\x81", "ú"}, {"u\xCC\x80", "ù"}, {"u\xCC\x82", "û"}, {"u\xCC\x88", "ü"},
    {"n\xCC\x83", "ñ"}, {"c\xCC\xA7", "ç"},
    
    // Formas precompuestas -> descompuestas (NFD)
    {"á", "a\xCC\x81"}, {"à", "a\xCC\x80"}, {"â", "a\xCC\x82"}, {"ä", "a\xCC\x88"},
    {"ã", "a\xCC\x83"}, {"å", "a\xCC\x8A"},
    {"é", "e\xCC\x81"}, {"è", "e\xCC\x80"}, {"ê", "e\xCC\x82"}, {"ë", "e\xCC\x88"},
    {"í", "i\xCC\x81"}, {"ì", "i\xCC\x80"}, {"î", "i\xCC\x82"}, {"ï", "i\xCC\x88"},
    {"ó", "o\xCC\x81"}, {"ò", "o\xCC\x80"}, {"ô", "o\xCC\x82"}, {"ö", "o\xCC\x88"},
    {"õ", "o\xCC\x83"},
    {"ú", "u\xCC\x81"}, {"ù", "u\xCC\x80"}, {"û", "u\xCC\x82"}, {"ü", "u\xCC\x88"},
    {"ñ", "n\xCC\x83"}, {"ç", "c\xCC\xA7"},
    
    {NULL, NULL}
};

// Tabla de eliminación de diacríticos
static const UnicodeMapping diacritic_removal[] = {
    // Vocales con diacríticos -> vocales base
    {"á", "a"}, {"à", "a"}, {"â", "a"}, {"ä", "a"}, {"ã", "a"}, {"å", "a"}, {"ā", "a"},
    {"Á", "A"}, {"À", "A"}, {"Â", "A"}, {"Ä", "A"}, {"Ã", "A"}, {"Å", "A"}, {"Ā", "A"},
    {"é", "e"}, {"è", "e"}, {"ê", "e"}, {"ë", "e"}, {"ē", "e"}, {"ė", "e"}, {"ę", "e"},
    {"É", "E"}, {"È", "E"}, {"Ê", "E"}, {"Ë", "E"}, {"Ē", "E"}, {"Ė", "E"}, {"Ę", "E"},
    {"í", "i"}, {"ì", "i"}, {"î", "i"}, {"ï", "i"}, {"ī", "i"}, {"į", "i"},
    {"Í", "I"}, {"Ì", "I"}, {"Î", "I"}, {"Ï", "I"}, {"Ī", "I"}, {"Į", "I"},
    {"ó", "o"}, {"ò", "o"}, {"ô", "o"}, {"ö", "o"}, {"õ", "o"}, {"ø", "o"}, {"ō", "o"}, {"ő", "o"},
    {"Ó", "O"}, {"Ò", "O"}, {"Ô", "O"}, {"Ö", "O"}, {"Õ", "O"}, {"Ø", "O"}, {"Ō", "O"}, {"Ő", "O"},
    {"ú", "u"}, {"ù", "u"}, {"û", "u"}, {"ü", "u"}, {"ū", "u"}, {"ů", "u"}, {"ų", "u"}, {"ű", "u"},
    {"Ú", "U"}, {"Ù", "U"}, {"Û", "U"}, {"Ü", "U"}, {"Ū", "U"}, {"Ů", "U"}, {"Ų", "U"}, {"Ű", "U"},
    {"ý", "y"}, {"ÿ", "y"}, {"ȳ", "y"},
    {"Ý", "Y"}, {"Ÿ", "Y"}, {"Ȳ", "Y"},
    
    // Consonantes con diacríticos
    {"ñ", "n"}, {"Ñ", "N"},
    {"ç", "c"}, {"Ç", "C"}, {"ć", "c"}, {"č", "c"}, {"ċ", "c"}, {"ĉ", "c"},
    {"Ć", "C"}, {"Č", "C"}, {"Ċ", "C"}, {"Ĉ", "C"},
    {"ł", "l"}, {"Ł", "L"}, {"ľ", "l"}, {"ĺ", "l"}, {"ļ", "l"},
    {"Ľ", "L"}, {"Ĺ", "L"}, {"Ļ", "L"},
    {"ř", "r"}, {"ŕ", "r"}, {"ŗ", "r"},
    {"Ř", "R"}, {"Ŕ", "R"}, {"Ŗ", "R"},
    {"ś", "s"}, {"š", "s"}, {"ş", "s"}, {"ș", "s"},
    {"Ś", "S"}, {"Š", "S"}, {"Ş", "S"}, {"Ș", "S"},
    {"ť", "t"}, {"ţ", "t"}, {"ț", "t"},
    {"Ť", "T"}, {"Ţ", "T"}, {"Ț", "T"},
    {"ź", "z"}, {"ž", "z"}, {"ż", "z"},
    {"Ź", "Z"}, {"Ž", "Z"}, {"Ż", "Z"},
    {"ğ", "g"}, {"Ğ", "G"},
    {"đ", "d"}, {"Đ", "D"},
    
    // Formas descompuestas (base + combinando)
    {"a\xCC\x81", "a"}, {"a\xCC\x80", "a"}, {"a\xCC\x82", "a"}, {"a\xCC\x88", "a"},
    {"a\xCC\x83", "a"}, {"a\xCC\x8A", "a"},
    {"e\xCC\x81", "e"}, {"e\xCC\x80", "e"}, {"e\xCC\x82", "e"}, {"e\xCC\x88", "e"},
    {"i\xCC\x81", "i"}, {"i\xCC\x80", "i"}, {"i\xCC\x82", "i"}, {"i\xCC\x88", "i"},
    {"o\xCC\x81", "o"}, {"o\xCC\x80", "o"}, {"o\xCC\x82", "o"}, {"o\xCC\x88", "o"},
    {"o\xCC\x83", "o"},
    {"u\xCC\x81", "u"}, {"u\xCC\x80", "u"}, {"u\xCC\x82", "u"}, {"u\xCC\x88", "u"},
    {"n\xCC\x83", "n"}, {"c\xCC\xA7", "c"},
    
    {NULL, NULL}
};

// Función auxiliar para buscar y reemplazar en tablas
static int apply_mapping_table(char* str, const UnicodeMapping* table) {
    if (!str || !table) return 0;
    
    int changes = 0;
    size_t len = strlen(str);
    char* temp = malloc(len * 4 + 1); // Buffer temporal más grande para expansiones
    if (!temp) return 0;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    
    while (in_pos < len) {
        int found = 0;
        
        // Buscar coincidencias en la tabla
        for (int i = 0; table[i].from != NULL; i++) {
            size_t from_len = strlen(table[i].from);
            if (in_pos + from_len <= len && 
                memcmp(str + in_pos, table[i].from, from_len) == 0) {
                
                // Encontrada coincidencia, aplicar reemplazo
                size_t to_len = strlen(table[i].to);
                memcpy(temp + out_pos, table[i].to, to_len);
                out_pos += to_len;
                in_pos += from_len;
                changes++;
                found = 1;
                break;
            }
        }
        
        if (!found) {
            // No hay coincidencia, copiar carácter original
            temp[out_pos++] = str[in_pos++];
        }
    }
    
    temp[out_pos] = '\0';
    
    // Copiar resultado de vuelta si hay cambios
    if (changes > 0) {
        strcpy(str, temp);
    }
    
    free(temp);
    return changes;
}

// 1. Case-folding Unicode avanzado
void unicode_case_fold(char* str) {
    if (!str) return;
    
    // Aplicar case-folding Unicode
    apply_mapping_table(str, case_folding_table);
    
    // Fallback a tolower para caracteres ASCII no cubiertos
    for (size_t i = 0; str[i]; i++) {
        if ((unsigned char)str[i] < 128) {
            str[i] = (char)tolower((unsigned char)str[i]);
        }
    }
}


void unicode_normalize(char* str, unicode_normalization_form form) {
    if (!str) return;
    
    if (form == UNICODE_NFC) {
        // Convertir formas descompuestas a precompuestas
        // Primero aplicamos las formas NFD->NFC
        for (int i = 0; canonical_normalization[i].from != NULL; i++) {
            if (strstr(canonical_normalization[i].from, "\xCC")) {
                // Es una forma descompuesta, convertir a precompuesta
                char* pos = str;
                while ((pos = strstr(pos, canonical_normalization[i].from)) != NULL) {
                    size_t from_len = strlen(canonical_normalization[i].from);
                    size_t to_len = strlen(canonical_normalization[i].to);
                    
                    if (to_len <= from_len) {
                        memcpy(pos, canonical_normalization[i].to, to_len);
                        memmove(pos + to_len, pos + from_len, 
                               strlen(pos + from_len) + 1);
                    }
                    pos += to_len;
                }
            }
        }
    } else if (form == UNICODE_NFD) {
        // Convertir formas precompuestas a descompuestas
        for (int i = 0; canonical_normalization[i].from != NULL; i++) {
            if (!strstr(canonical_normalization[i].from, "\xCC")) {
                // Es una forma precompuesta, convertir a descompuesta
                char* temp = malloc(strlen(str) * 3 + 1);
                if (!temp) return;
                
                strcpy(temp, str);
                char* pos = temp;
                char* out = str;
                size_t out_pos = 0;
                
                while (*pos) {
                    int found = 0;
                    for (int j = 0; canonical_normalization[j].from != NULL; j++) {
                        if (!strstr(canonical_normalization[j].from, "\xCC")) {
                            size_t from_len = strlen(canonical_normalization[j].from);
                            if (memcmp(pos, canonical_normalization[j].from, from_len) == 0) {
                                size_t to_len = strlen(canonical_normalization[j].to);
                                memcpy(out + out_pos, canonical_normalization[j].to, to_len);
                                out_pos += to_len;
                                pos += from_len;
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (!found) {
                        out[out_pos++] = *pos++;
                    }
                }
                out[out_pos] = '\0';
                free(temp);
            }
        }
    }
}

// 3. Eliminación de diacríticos (con flag opcional)
void remove_diacritics(char* str, int enable_removal) {
    if (!str || !enable_removal) return;
    
    apply_mapping_table(str, diacritic_removal);
    
    // Eliminar marcas diacríticas combinadas restantes (rango Unicode 0300-036F)
    char* temp = malloc(strlen(str) + 1);
    if (!temp) return;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    size_t len = strlen(str);
    
    while (in_pos < len) {
        unsigned char c1 = (unsigned char)str[in_pos];
        
        // Detectar secuencias UTF-8 de marcas combinadas
        if (c1 == 0xCC && in_pos + 1 < len) {
            unsigned char c2 = (unsigned char)str[in_pos + 1];
            if (c2 >= 0x80 && c2 <= 0xBF) {
                // Es una marca diacrítica combinada, omitir
                in_pos += 2;
                continue;
            }
        } else if (c1 == 0xCD && in_pos + 1 < len) {
            unsigned char c2 = (unsigned char)str[in_pos + 1];
            if (c2 >= 0x80 && c2 <= 0xAF) {
                // Es una marca diacrítica combinada, omitir
                in_pos += 2;
                continue;
            }
        }
        
        temp[out_pos++] = str[in_pos++];
    }
    
    temp[out_pos] = '\0';
    strcpy(str, temp);
    free(temp);
}

// Función combinada que aplica todas las normalizaciones
void unicode_normalize_full(char* str, int remove_diacritics_flag) {
    if (!str) return;
    
    // 1. Normalización canónica a NFC
    unicode_normalize(str, UNICODE_NFC);
    
    // 2. Case-folding Unicode
    unicode_case_fold(str);
    
    // 3. Eliminación de diacríticos (opcional)
    remove_diacritics(str, remove_diacritics_flag);
    
    // 4. Limpieza de espacios (reutilizando función existente)
    squeezeSpaces(str);
}

// Funciones originales mantenidas para compatibilidad
void buildNormalizationTable(unsigned char table[256]) {
    const char* from = "ÁÀÂÄáàâäÉÈÊËéèêëÍÌÎÏíìîïÓÒÔÖóòôöÚÙÛÜúùûüÑñÇç";
    const char* to   = "AAAAaaaaEEEEeeeeIIIIiiiiOOOOooooUUUUuuuuNnCc";
    
    for (int i = 0; i < 256; i++) {
        table[i] = i;
    }
    
    for (int c = 'A'; c <= 'Z'; c++) {
        table[c] = c - 'A' + 'a';
    }
    
    for (size_t i = 0; from[i] != '\0'; i++) {
        table[(unsigned char)from[i]] = to[i];
    }
}

void normalizeString(char* str) {
    static unsigned char table[256];
    static int initialized = 0;
    
    if (!initialized) {
        for (int i = 0; i < 256; i++) {
            table[i] = i;
        }
        
        for (int c = 'A'; c <= 'Z'; c++) {
            table[c] = c - 'A' + 'a';
        }
        
        const char* from = "ÁÀÂÄÃáàâäãÉÈÊËéèêëÍÌÎÏíìîïÓÒÔÖÕóòôöõÚÙÛÜúùûüÑñÇç";
        const char* to   = "AAAAAaaaaaEEEEeeeeIIIIiiiiOOOOOoooooUUUUuuuuNnCc";
        
        for (size_t i = 0; from[i] != '\0'; i++) {
            table[(unsigned char)from[i]] = to[i];
        }
        initialized = 1;
    }
    
    for (size_t i = 0; str[i]; i++) {
        str[i] = (char)table[(unsigned char)str[i]];
    }
}

char* loadFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("loadFile: no se pudo abrir el archivo");
        return NULL;
    }
    
    unsigned char bom[3];
    size_t bom_size = fread(bom, 1, 3, fp);
    int has_bom = (bom_size == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF);
    
    if (fseek(fp, 0, SEEK_END)) {
        perror("loadFile: fseek falló");
        fclose(fp);
        return NULL;
    }
    long file_size = ftell(fp);
    if (file_size < 0) {
        perror("loadFile: ftell devolvió error");
        fclose(fp);
        return NULL;
    }
    
    size_t content_size = (size_t)file_size;
    if (has_bom) {
        if (file_size >= 3) {
            content_size -= 3;
            fseek(fp, 3, SEEK_SET);
        } else {
            content_size = 0;
        }
    } else {
        rewind(fp);
    }

    char* buffer = malloc(content_size + 1);
    if (!buffer) {
        fprintf(stderr, "loadFile: malloc devolvió NULL para %zu bytes\n", content_size + 1);
        fclose(fp);
        return NULL;
    }
    
    size_t readBytes = fread(buffer, 1, content_size, fp);
    fclose(fp);
    
    if (readBytes != content_size) {
        fprintf(stderr, "loadFile: leídos %zu de %zu bytes\n", readBytes, content_size);
        free(buffer);
        return NULL;
    }
    
    buffer[content_size] = '\0';
    return buffer;
}

void toLowerInPlace(char* s) {
    for (size_t i = 0; s[i]; i++)
        s[i] = (char)tolower((unsigned char)s[i]);
}

void squeezeSpaces(char* s) {
    size_t i = 0, o = 0;
    int in_space = 0;
    while (s[i]) {
        if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t' || s[i] == '\r') {
            if (!in_space) {
                s[o++] = ' ';
                in_space = 1;
            }
            i++;
        } else {
            in_space = 0;
            s[o++] = s[i++];
        }
    }
    if (o > 0 && s[o - 1] == ' ') o--;
    s[o] = '\0';
}

void convertir_a_minusculas(char *palabra) {
    int j = 0;
    for (int i = 0; palabra[i]; ) {
        unsigned char c = palabra[i];
        if (c == 0xC3) { 
            unsigned char c2 = palabra[i+1];
            if (c2) {
                i++;
                switch(c2) {
                    case 0xA1: case 0xA0: case 0xA4: case 0xA2: case 0xA3: case 0xA5: 
                        palabra[j++] = 'a'; break;
                    case 0xA9: case 0xA8: case 0xAB: case 0xAA: 
                        palabra[j++] = 'e'; break;
                    case 0xAD: case 0xAC: case 0xAF: case 0xAE: 
                        palabra[j++] = 'i'; break;
                    case 0xB3: case 0xB2: case 0xB6: case 0xB4: case 0xB5: 
                        palabra[j++] = 'o'; break;
                    case 0xBA: case 0xB9: case 0xBC: case 0xBB: 
                        palabra[j++] = 'u'; break;
                    case 0xB1: 
                        palabra[j++] = 'n'; break;
                    default: 
                        i++;
                        continue;
                }
                i++;
            } else {
                i++;
            }
        } else if (c >= 128) { 
            i++;
        } else { 
            palabra[j++] = tolower(c);
            i++;
        }
    }
    palabra[j] = '\0';
}

void limpiar_palabra(char *palabra) {
    int j = 0;
    for (int i = 0; palabra[i]; i++) {
        unsigned char c = palabra[i];
        if (isalnum(c) || c == ' ') {
            palabra[j++] = c;
        }
    }
    palabra[j] = '\0';
}

static void decodeEntity(const char* start, char** output) {
    if (*start == '#') {
        start++;
        int base = 10;
        if (*start == 'x' || *start == 'X') {
            start++;
            base = 16;
        }
        
        char* end;
        long code = strtol(start, &end, base);
        
        if (*end == ';' && code != 0) {
            if (code < 0x80) {
                *(*output)++ = (char)code;
            } else if (code < 0x800) {
                *(*output)++ = 0xC0 | (code >> 6);
                *(*output)++ = 0x80 | (code & 0x3F);
            } else if (code < 0x10000) {
                *(*output)++ = 0xE0 | (code >> 12);
                *(*output)++ = 0x80 | ((code >> 6) & 0x3F);
                *(*output)++ = 0x80 | (code & 0x3F);
            } else {
                *(*output)++ = 0xF0 | (code >> 18);
                *(*output)++ = 0x80 | ((code >> 12) & 0x3F);
                *(*output)++ = 0x80 | ((code >> 6) & 0x3F);
                *(*output)++ = 0x80 | (code & 0x3F);
            }
            return;
        }
    } else {
        const char *entity = start;
        size_t len = strcspn(start, ";");
        
        if (strncmp(entity, "amp", len) == 0) *(*output)++ = '&';
        else if (strncmp(entity, "lt", len) == 0) *(*output)++ = '<';
        else if (strncmp(entity, "gt", len) == 0) *(*output)++ = '>';
        else if (strncmp(entity, "quot", len) == 0) *(*output)++ = '"';
        else if (strncmp(entity, "apos", len) == 0) *(*output)++ = '\'';
        else if (strncmp(entity, "nbsp", len) == 0) *(*output)++ = ' ';
        else if (strncmp(entity, "copy", len) == 0) { *(*output)++ = 0xC2; *(*output)++ = 0xA9; }
        else {
            *(*output)++ = '&';
            while (*start && *start != ';') *(*output)++ = *start++;
            if (*start == ';') *(*output)++ = ';';
            return;
        }
    }
    while (*start && *start != ';') start++;
}

char* stripHTML(const char* html) {
    size_t len = strlen(html);
    char* output = malloc(len + 1);
    if (!output) return NULL;

    char* out = output;
    int in_tag = 0;
    int in_comment = 0;
    int in_script = 0;

    for (size_t i = 0; i < len; i++) {
        if (!in_tag && !in_comment && !in_script) {
            if (html[i] == '<') {
                if (i + 4 < len && strncmp(html + i, "<!--", 4) == 0) {
                    in_comment = 1;
                    i += 3;
                } else if (i + 7 < len && strncmp(html + i, "<script", 7) == 0) {
                    in_script = 1;
                    i += 6;
                } else {
                    in_tag = 1;
                }
            } else if (html[i] == '&') {
                const char* start = html + i + 1;
                decodeEntity(start, &out);
                while (html[i] != ';' && html[i] != '\0') i++;
            } else {
                *out++ = html[i];
            }
        } else if (in_comment) {
            if (i + 3 < len && strncmp(html + i, "-->", 3) == 0) {
                in_comment = 0;
                i += 2;
            }
        } else if (in_script) {
            if (i + 9 < len && strncmp(html + i, "</script>", 9) == 0) {
                in_script = 0;
                i += 8;
            }
        } else if (in_tag) {
            if (html[i] == '>') {
                in_tag = 0;
            }
        }
    }
    
    *out = '\0';
    return output;
}