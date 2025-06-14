import os
import pandas as pd
import matplotlib.pyplot as plt

script_dir = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(script_dir, "../results/benchmark.csv")

#verifica existencia del archivo
if not os.path.exists(csv_path):
    raise FileNotFoundError(f"No se encontró el archivo CSV en: {csv_path}")

#carga datos
df = pd.read_csv(csv_path)

#convierte tiempo a float
df['tiempo_s'] = pd.to_numeric(df['tiempo_s'], errors='coerce')

#verifica que todas las columnas necesarias esten presentes
required_cols = {'algoritmo', 'corpus', 'tamano_bytes', 'tiempo_s', 'comparaciones', 'shifts', 'patron'}
missing_cols = required_cols - set(df.columns)
if missing_cols:
    raise ValueError(f"Faltan las siguientes columnas en el CSV: {', '.join(missing_cols)}")

# Graficar tiempo por tamaño de archivo para cada patrón
output_dir = os.path.join(script_dir, "../results/graficos")
os.makedirs(output_dir, exist_ok=True)

for pattern in df['patron'].unique():
    plt.figure()
    for alg in df['algoritmo'].unique():
        sub_df = df[(df['algoritmo'] == alg) & (df['patron'] == pattern)]
        if not sub_df.empty:
            plt.plot(sub_df['tamano_bytes'], sub_df['tiempo_s'], marker='o', label=alg)

    plt.title(f"Tiempo de búsqueda - Patrón: \"{pattern}\"")
    plt.xlabel("Tamaño del archivo (bytes)")
    plt.ylabel("Tiempo (segundos)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    output_file = os.path.join(output_dir, f"tiempo_patron_{pattern.replace(' ', '_')}.png")
    plt.savefig(output_file)
    plt.close()

print(f"✅ Gráficos generados en: {output_dir}")