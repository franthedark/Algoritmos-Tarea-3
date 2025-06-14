#!/usr/bin/env bash
set -euo pipefail

# --------------------------------------------------------------------------
# Configuración
# --------------------------------------------------------------------------
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CMD="$ROOT/build/buscador"
OUTPUT="$ROOT/results/benchmark.csv"
mkdir -p "$(dirname "$OUTPUT")"

# Algoritmos
ALGS=( kmp bm shiftand )

# Corpus de prueba
CORPORA=(
  "$ROOT/docs/corpus/sample_100k.txt"
  "$ROOT/docs/corpus/sample_500k.txt"
  "$ROOT/docs/corpus/sample_1m.txt"
)

# Patrones de búsqueda
PATTERNS=(
  "lorem"
  "amet consectetur"
  "dolor"
)

# Cabecera CSV con patrón incluido
echo "algoritmo,patron,corpus,tamano_bytes,tiempo_s,comparaciones,shifts" > "$OUTPUT"

# --------------------------------------------------------------------------
# Loop principal
# --------------------------------------------------------------------------
for corpus in "${CORPORA[@]}"; do
  if [ ! -f "$corpus" ]; then
    echo "ERROR: Corpus no existe: $corpus" >&2
    exit 1
  fi
  size=$(stat -c%s "$corpus")
  echo "-- Corpus: $(basename "$corpus") ($size bytes)"

  for pattern in "${PATTERNS[@]}"; do
    for alg in "${ALGS[@]}"; do
      echo "   * $alg '$pattern'"

      out=$(mktemp)

      start=$(date +%s.%N)
      "$CMD" "$alg" "$pattern" "$corpus" >"$out"
      end=$(date +%s.%N)
      time_s=$(echo "scale=20; $end - $start" | bc)

      comp=0
      shifts=0
      case "$alg" in
        kmp)
          comp=$(grep -oP '(?<=Comparaciones: )\d+' "$out" | head -1 || echo 0)
          ;;
        bm)
          comp=$(grep -oP '(?<=Comparaciones: )\d+' "$out" | head -1 || echo 0)
          shifts=$(grep -oP '(?<=Shifts: )\d+' "$out" | head -1 || echo 0)
          ;;
        shiftand)
          comp=$(grep -oP '(?<=Caracteres procesados: )\d+' "$out" | head -1 || echo 0)
          ;;
      esac

      rm -f "$out"

      # Agregar línea con patrón incluido
      echo "$alg,\"$pattern\",$(basename "$corpus"),$size,$time_s,$comp,$shifts" >> "$OUTPUT"
      echo "      -> tiempo=${time_s}s, comp=${comp}, shifts=${shifts}"
    done
  done
done

echo "=== Benchmark completado. Resultados en $OUTPUT ==="