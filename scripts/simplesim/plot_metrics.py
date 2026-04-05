import os
import re
import sys
import pandas as pd
import matplotlib.pyplot as plt

# Usage: python plot_metrics.py <metrics_csv>

def sanitize_filename(name):
    """Remove/replace characters that are unsafe in filenames or shell contexts."""
    # Strip $(...) wrapper (e.g. $(speedUp) -> speedUp)
    name = re.sub(r'^\$\((.*)\)$', r'\1', name)
    # Replace any remaining special characters with underscores
    name = re.sub(r'[^\w\-.]', '_', name)
    return name

def plot_metrics(csv_path):
    df = pd.read_csv(csv_path)
    metrics_dir = os.path.dirname(csv_path)
    if not metrics_dir:
        metrics_dir = '.'
    base = os.path.splitext(os.path.basename(csv_path))[0]

    # The first column is config, the rest are metrics
    configs = df.iloc[:, 0].astype(str)
    metric_cols = df.columns[1:]

    for metric in metric_cols:
        plt.figure(figsize=(8, 5))
        values = df[metric]
        # Try to convert to float, else NaN
        values = pd.to_numeric(values, errors='coerce')
        plt.bar(configs, values, color='skyblue')
        plt.xlabel('Config')
        plt.ylabel(metric)
        plt.title(f'{metric} vs Config')
        plt.xticks(rotation=45, ha='right')
        plt.tight_layout()
        safe_metric = sanitize_filename(metric)
        out_path = os.path.join(metrics_dir, f'{base}_{safe_metric}_bar.png')
        plt.savefig(out_path)
        plt.close()
        print(f'Wrote: {out_path}')

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot_metrics.py <metrics_csv>")
        sys.exit(1)
    plot_metrics(sys.argv[1])
