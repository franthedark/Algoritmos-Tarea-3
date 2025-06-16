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

### Compilación

Compila los módulos y genera el ejecutable en `build/buscador`

```bash
make
```

### Ayuda

Muestra un resumen de todos los comandos disponibles

```bash
make run
```

### Búsqueda de patrones

Ejecuta la búsqueda exacta con el algoritmo elegido (KMP, Boyer–Moore o Shift-And) sobre un fichero de texto o HTML (debe existir en el directorio `docs/` o en la ruta actual):

* **KMP**

  ```bash
  make run-kmp PAT="patrón" FILE=archivo.txt
  ```
* **Boyer–Moore**

  ```bash
  make run-bm PAT="patrón" FILE=archivo.txt
  ```
* **Shift-And**

  ```bash
  make run-shiftand PAT="patrón" FILE=archivo.txt
  ```

### Gestión de índices

Construye y consulta índices invertidos para búsquedas ultra-rápidas:

* **Crear índice**

  ```bash
  make create-index DIR=docs [INDEX=mi_indice.idx]
  ```
* **Buscar en el índice**

  ```bash
  make search-index TERM="término" [INDEX=mi_indice.idx]
  ```
* **Mostrar información del índice**

  ```bash
  make index-info [INDEX=mi_indice.idx]
  ```
* **Exportar índice a texto**

  ```bash
  make export-index OUTPUT=salida.txt [INDEX=mi_indice.idx]
  ```
* **Hacer copia de seguridad**

  ```bash
  make backup-index [INDEX=mi_indice.idx] [BACKUP_DIR=backups/]
  ```

### Limpieza

* **Borrar objetos compilados**

  ```bash
  make clean
  ```
* **Borrar todo (incluye índices y backups)**

  ```bash
  make clean-all
  ```
## 📦 Utilidades y Scripts (`tools/`)

En el directorio [`tools/`](tools/) hemos agrupado todas las herramientas auxiliares para:

- **Descargar y preparar** corpora de texto de prueba (`fetch_corpus.sh`).
- **Ejecutar benchmarks** de rendimiento (`benchmark.sh`).
- **Visualizar resultados** con gráficos (`graficar_benchmark.py`).

> Para ver instrucciones detalladas, visita  
> [tools/README.md](tools/README.md)
