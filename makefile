#makefile para proyecto de busqueda exacta con tres algoritmos

CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra
OBJDIR = obj
INC_DIRS = -Iincs

#fuentes .c en src/
SRCS = src/main.c \
       src/KMP.c \
       src/boyer_moore.c \
       src/shift_and.c \
       src/utils.c

#convierte cada src/archivo.c -> obj/archivo.o
OBJS = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRCS))

all: setup $(OBJDIR) buscador

setup:
	@mkdir -p $(OBJDIR)

#cada src/*.c compila a obj/*.o
$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

buscador: $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) -o buscador $(OBJS)

clean:
	rm -rf $(OBJDIR) buscador

.PHONY: all clean setup