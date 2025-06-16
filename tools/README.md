# Herramientas y Scripts de Soporte

Este directorio agrupa todos los scripts y utilidades para generar datos de prueba, medir el rendimiento de los algoritmos y visualizar los resultados.

---

## Requisitos

1. **Shell**: bash (o compatible POSIX).  
2. **GNU coreutils**: `date`, `stat`, `bc`, `awk`, `grep`, `mktemp`, etc.  
3. **Python 3** (opcional, para gráficas):  
   - pandas  
   - matplotlib  
```
   pip install pandas matplotlib
```

---

# Herramientas y Scripts (`tools/`)

En este directorio encontrarás una serie de utilidades para preparar corpora de prueba, generar datos sintéticos, ejecutar benchmarks y visualizar resultados.

---

## fetch_corpus.sh

Descarga y prepara automáticamente varios conjuntos de prueba en `docs/corpus/`:

1. **Project Gutenberg**  
   - Obtiene los textos de “Pride and Prejudice” (ID 1342) y “Moby Dick” (ID 2701) en formato HTML.  
   - Aplica un filtro básico `sed 's/<[^>]\+>//g'` (stripHTML) para extraer solo el texto plano.  
   - Guarda los resultados en `docs/corpus/1342.txt` y `docs/corpus/2701.txt`.

2. **SMS Spam Collection**  
   - Descarga el CSV etiquetado de mensajes ham/spam.  
   - Guarda el archivo en `docs/corpus/sms_spam.csv`.

3. **Simple English Wikipedia**  
   - Descarga el volcado XML de la Wikipedia en inglés simple.  
   - Extrae los primeros 5000 renglones como muestra en `docs/corpus/sample_wiki.xml`.

4. **Generación de `docs/corpus/README.md`**  
   - Crea un README con la descripción de cada archivo generado.

> **Nota:** si quieres utilizar otros textos o IDs de Gutenberg, o cambiar el tamaño de la muestra de Wikipedia, modifica directamente las variables y URLs dentro de `fetch_corpus.sh`.

---

## generacsv.sh

Genera datos sintéticos de prueba en formato CSV (por ejemplo, para pruebas de indexación o benchmarking de carga).

---

## benchmark.sh

Ejecuta cada algoritmo (`kmp`, `bm`, `shiftand`) sobre distintos tamaños de texto y patrones, mide:

- Tiempo de ejecución (con alta precisión).
- Número de comparaciones.
- Número de shifts (para Boyer–Moore).

Guarda los resultados en `results/benchmark.csv`.

---

## graficar_benchmark.py

Carga `results/benchmark.csv` y genera gráficos lineales de:

- Tiempo vs. tamaño de texto
- Comparaciones vs. tamaño de texto
- (opcional) Separado por patrón de búsqueda

Produce archivos PNG en el mismo directorio.

---

## Uso

Desde la raíz del proyecto:

```
cd tools
```
**Prepara los corpus de prueba**
```
./fetch_corpus.sh
```
**(opcional) Genera CSVs sintéticos**
```
./generacsv.sh
```
**Ejecuta el benchmark**
```
./benchmark.sh
```
**Grafica los resultados**
```
python3 graficar_benchmark.py
```
> **Nota final:** si deseas emplear otros archivos de muestra o corpora distintos, revisa y adapta los scripts correspondientes en este directorio.
