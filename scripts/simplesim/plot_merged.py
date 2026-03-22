import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import glob

def collect_metric_files(build_dir, simulator):
    # Find all metrics CSVs for the given simulator
    pattern = os.path.join(build_dir, '*', simulator, 'metrics', f'*_{simulator}_metrics.csv')
    return glob.glob(pattern)

def merge_and_plot(build_dir, simulator, metric_idx=0, out_dir=None, y_label=None, plot_title=None):
    metric_files = collect_metric_files(build_dir, simulator)
    if not metric_files:
        print(f"No metrics files found for simulator '{simulator}' in {build_dir}")
        return
    # Read all data
    data = {}
    configs = None
    metric_names = None
    for csv_path in metric_files:
        benchmark = os.path.basename(csv_path).split('_')[0]
        df = pd.read_csv(csv_path)
        if configs is None:
            configs = df.iloc[:, 0].astype(str)
        if metric_names is None:
            metric_names = list(df.columns[1:])
        # For each metric column, store values
        for i, metric in enumerate(metric_names):
            if metric not in data:
                data[metric] = {}
            # Use the metric_idx if specified
            if i == metric_idx:
                data[metric][benchmark] = pd.to_numeric(df.iloc[:, i+1], errors='coerce').values
    # Plot for the selected metric
    metric = metric_names[metric_idx]
    plt.figure(figsize=(8, 5))
    bar_width = 0.15
    indices = range(len(configs))
    colors = plt.cm.tab10.colors
    for idx, (bench, values) in enumerate(data[metric].items()):
        plt.bar([i + idx*bar_width for i in indices], values, bar_width, label=bench, color=colors[idx % len(colors)])
    plt.xlabel('Config')
    plt.ylabel(y_label if y_label else metric)
    plt.title(plot_title if plot_title else f'{metric} vs Config (all benchmarks)')
    plt.xticks([i + bar_width*(len(data[metric])/2-0.5) for i in indices], configs, rotation=45, ha='right')
    plt.legend()
    plt.tight_layout()
    if out_dir is None:
        out_dir = os.path.join(build_dir, 'plots')
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, f'{simulator}_{metric}_merged.png')
    plt.savefig(out_path)
    plt.close()
    print(f'Wrote: {out_path}')

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Plot merged metrics for all benchmarks of a simulator.")
    parser.add_argument('build_dir', help='Build directory (e.g., ../../build)')
    parser.add_argument('simulator', help='Simulator name (e.g., cache)')
    parser.add_argument('metric_index', nargs='?', type=int, default=0, help='Metric column index (default: 0)')
    parser.add_argument('--ylabel', default=None, help='Y axis label (default: metric name)')
    parser.add_argument('--title', default=None, help='Plot title (default: metric vs Config)')
    args = parser.parse_args()
    merge_and_plot(args.build_dir, args.simulator, args.metric_index, y_label=args.ylabel, plot_title=args.title)
