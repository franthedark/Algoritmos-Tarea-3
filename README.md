# Proyecto de Procesamiento de Texto - Diseño de Algoritmos Tarea 3
Integrantes: Diego Galindo y Francisco Mercado
---
Este repositorio contiene el código fuente del proyecto de Procesamiento de Texto y Análisis de Patrones, desarrollado en C (estándar C11/C99) para el curso Diseño de Algoritmos de la Universidad de Magallanes. El objetivo es implementar y comparar distintos algoritmos de búsqueda de patrones, estructuras de datos para indexación y técnicas de preprocesamiento de texto, integrándolos en una herramienta de línea de comandos que permita:

- Indexar colecciones de documentos.

- Realizar búsquedas exactas y aproximadas de palabras o frases.

- Generar estadísticas y análisis de similaridad entre documentos.

- Medir y comparar el rendimiento de cada algoritmo.
---

# Roles del proyecto:

**Francisco Mercado**

Desarrollador principal de algoritmos de búsqueda

- Responsable de la implementación y optimización de KMP, Boyer–Moore y Shift-And

- Diseña y analiza la “función de fallos” (LPS) y las heurísticas de bad‐character/shift-and

- Coordina la integración de los algoritmos con el sistema de carga y preprocesamiento de documentos

**Diego Galindo**

Encargado de carga, preprocesamiento e indexación

- Implementa funciones de lectura de archivos (texto/HTML), normalización y limpieza (loadFile, stripHTML, etc.)

- Desarrolla la construcción del índice invertido y la persistencia de datos en disco

- Se ocupa de las pruebas de rendimiento, validación de resultados y documentación de casos de uso

**Nota: estos roles definen lo principal, pero en la práctica puede variar si alguno está con dificultades**

# Estructura de carpetas
```
├── src/         # Código fuente (.c) de cada módulo
├── incs/        # Archivos de cabecera (.h)
├── tools/       # Scripts y utilidades (generación de datos, etc.)
├── docs/        # Documentación adicional y casos de prueba
├── Makefile     # Instrucciones de compilación
└── README.md    # Este archivo
```

## Ejecución del proyecto

**Compilar todo**  
 Compila los módulos y genera el ejecutable en `build/buscador`
  
```
make
```    
**Mostrar ayuda básica**  
 Muestra un mensaje con instrucciones de uso
 
```
make run
```    
**Buscar con KMP**  
 Ejecuta el algoritmo Knuth–Morris–Pratt sobre el archivo indicado
 
```
make run-kmp PAT="patrón" FILE=archivo.txt
```    
**Buscar con Boyer–Moore**  
 Ejecuta el algoritmo Boyer–Moore sobre el archivo indicado
 
```
make run-bm PAT="patrón" FILE=archivo.txt
```    
**Buscar con Shift-And**  
 Ejecuta el algoritmo Shift-And sobre el archivo indicado
 
```
make run-sa PAT="patrón" FILE=archivo.txt
```    
**Limpiar artefactos**  
 Elimina los directorios build/ y obj/ con todos los objetos y el ejecutable
 
```
make clean
```
