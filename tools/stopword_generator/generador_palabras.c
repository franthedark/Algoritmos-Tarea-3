/* @Authors : Diego Galindo, Francisco Mercado, Benjamin Sanhueza, Duvan Figueroa */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORDS 1000
#define MAX_WORD_LENGTH 100

//funcion leer palabras desde un archivo
int leer_palabras(const char *nombre_archivo, char palabras[MAX_WORDS][MAX_WORD_LENGTH]) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) {
        printf("Error al abrir el archivo %s.\n", nombre_archivo);
        return -1;
    }

    int contador = 0;
    while (fscanf(archivo, "%99s", palabras[contador]) == 1 && contador < MAX_WORDS) {
        contador++;
    }

    fclose(archivo);
    return contador;
}

void generar_documento(const char *nombre_archivo, char palabras[MAX_WORDS][MAX_WORD_LENGTH], int total_palabras, int palabras_por_documento);

int main() {
    char palabras[MAX_WORDS][MAX_WORD_LENGTH];
    int total_palabras;

    char archivo_fuente[] = "palabras.txt"; 
    total_palabras = leer_palabras(archivo_fuente, palabras);

    if (total_palabras == -1) {
        return 1;
    }

    int num_documentos;
    printf("Ingrese la cantidad de documentos a generar: ");
    scanf("%d", &num_documentos);

    srand(time(NULL));

    for (int i = 0; i < num_documentos; i++) {
        char nombre_archivo[50];
        snprintf(nombre_archivo, sizeof(nombre_archivo), "../docs/documento_%d.txt", i + 1);

        int palabras_por_documento = rand() % 50 + 1; //numero aleatorio de palabras (1 a 50)
        generar_documento(nombre_archivo, palabras, total_palabras, palabras_por_documento);
    }

    printf("Todos los documentos se han generado correctamente.\n");

    return 0;
}


void generar_documento(const char *nombre_archivo, char palabras[MAX_WORDS][MAX_WORD_LENGTH], int total_palabras, int palabras_por_documento) {
    FILE *archivo = fopen(nombre_archivo, "w");
    if (!archivo) {
        printf("Error al crear el archivo %s.\n", nombre_archivo);
        return;
    }

    for (int i = 0; i < palabras_por_documento; i++) {
        int indice = rand() % total_palabras; // Seleccion aleatoria de palabras
        fprintf(archivo, "%s\n", palabras[indice]);
    }

    fclose(archivo);
    printf("Archivo %s generado con %d palabras.\n", nombre_archivo, palabras_por_documento);
}