#ifndef BOYER_MOORE_H
#define BOYER_MOORE_H

#include <stddef.h>

//construye la tabla bad‐character para Boyer–Moore
void preprocessBadChar(const char *pat, int badChar[256]);

//construye la tabla good‐suffix para Boyer–Moore
void preprocessGoodSuffix(const char *pat, size_t M, size_t *shiftGS);

//busca todas las ocurrencias con boyer–moore (bad‐char + good‐suffix)
void searchBoyerMoore(const char *pattern, const char *text);

#endif