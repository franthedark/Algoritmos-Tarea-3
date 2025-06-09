#include "html_parser.h"

int main(int argc, char* argv[]) {
    // Verificar argumentos
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = (argc == 3) ? argv[2] : NULL;
    
    printf("Procesando archivo HTML: %s\n", input_file);
    
    // Leer archivo HTML
    char* html_content = read_file(input_file);
    if (!html_content) {
        return 1;
    }
    
    printf("Archivo leído correctamente (%zu bytes)\n", strlen(html_content));
    
    // Quitar etiquetas HTML
    char* plain_text = strip_html_tags(html_content);
    if (!plain_text) {
        fprintf(stderr, "Error: No se pudo procesar el contenido HTML\n");
        free(html_content);
        return 1;
    }
    
    printf("Etiquetas HTML removidas\n");
    
    // Mostrar resultado o guardarlo
    if (output_file) {
        save_text_file(output_file, plain_text);
    } else {
        printf("\n--- CONTENIDO PROCESADO ---\n");
        printf("%s\n", plain_text);
        printf("--- FIN DEL CONTENIDO ---\n");
    }
    
    // Liberar memoria
    free(html_content);
    free(plain_text);
    
    printf("Procesamiento completado exitosamente\n");
    return 0;
}