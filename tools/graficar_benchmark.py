import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

script_dir = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(script_dir, "../results/benchmark.csv")

# Verifica existencia del archivo
if not os.path.exists(csv_path):
    raise FileNotFoundError(f"No se encontró el archivo CSV en: {csv_path}")

# Carga datos
df = pd.read_csv(csv_path)

# Convierte columnas numéricas
df['tiempo_s'] = pd.to_numeric(df['tiempo_s'], errors='coerce')
df['tamano_bytes'] = pd.to_numeric(df['tamano_bytes'], errors='coerce')

# Verifica columnas necesarias
required_cols = {'algoritmo', 'tamano_bytes', 'patron', 'tiempo_s'}
missing_cols = required_cols - set(df.columns)
if missing_cols:
    raise ValueError(f"Faltan columnas en el CSV: {', '.join(missing_cols)}")

# Mapeo de tamaños a etiquetas
size_labels = {
    102580: "100K",
    512008: "500K",
    1048992: "1M"
}

# Graficar tiempo por tamaño de archivo
output_dir = os.path.join(script_dir, "../results/graficos")
os.makedirs(output_dir, exist_ok=True)

for pattern in df['patron'].unique():
    plt.figure(figsize=(10, 6))
    
    for alg in df['algoritmo'].unique():
        sub_df = df[(df['algoritmo'] == alg) & (df['patron'] == pattern)]
        if not sub_df.empty:
            # Ordenar por tamaño
            sub_df = sub_df.sort_values('tamano_bytes')
            
            plt.plot(
                sub_df['tamano_bytes'], 
                sub_df['tiempo_s'], 
                marker='o', 
                linestyle='-', 
                label=alg
            )

    # Usar etiquetas de tamaño consistentes
    sizes = sorted(df['tamano_bytes'].unique())
    labels = [size_labels.get(size, f"{size/1000:.0f}K") for size in sizes]
    
    plt.title(f"Tiempo de búsqueda - Patrón: {pattern}")
    plt.xlabel("Tamaño del archivo (bytes)")
    plt.ylabel("Tiempo (segundos)")
    
    plt.xticks(sizes, labels)
    plt.gca().yaxis.set_major_formatter(ticker.FormatStrFormatter('%.3f'))
    
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    
    # Nombre de archivo seguro
    safe_pattern = ''.join(c for c in pattern if c.isalnum() or c in " _-")
    output_file = os.path.join(output_dir, f"tiempo_patron_{safe_pattern}.png")
    plt.savefig(output_file)
    plt.close()

print(f"✅ Gráficos generados en: {output_dir}")