import json
import os
import sys
import re
import csv

# Usage: python extract_metrics.py config.json metrics.json output.csv

def extract_value_from_simout(simout_path, metric_key, debug_log=None):
    # Search for the metric_key in the file and return the first float value after it (same or next non-empty line)
    if not os.path.exists(simout_path):
        if debug_log:
            debug_log.write(f"File not found: {simout_path}\n")
        return None
    float_pattern = re.compile(r'([-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)')
    # Match metric_key as a whole word (not substring)
    # Allow for substring match if metric_key is a suffix (e.g., bpred_addr_rate matches bpred_bimod.bpred_addr_rate)
    key_pattern = re.compile(rf'(^|\s)[\w\.]*{re.escape(metric_key)}(\s|:|=|$)')
    with open(simout_path) as f:
        lines = f.readlines()
    for idx, line in enumerate(lines):
        if key_pattern.search(line):
            if debug_log:
                debug_log.write(f"Found key '{metric_key}' in {simout_path} at line {idx+1}: {line.strip()}\n")
            # Try to find float on the same line after the key
            after_key = line.split(metric_key, 1)[1]
            match = float_pattern.search(after_key)
            if match:
                if debug_log:
                    debug_log.write(f"  Found float after key: {match.group(1)}\n")
                return float(match.group(1))
            # If not found, try next non-empty, non-comment line(s)
            next_idx = idx + 1
            while next_idx < len(lines):
                next_line = lines[next_idx].strip()
                if next_line and not next_line.startswith('#'):
                    match = float_pattern.search(next_line)
                    if match:
                        if debug_log:
                            debug_log.write(f"  Found float on next line: {match.group(1)}\n")
                        return float(match.group(1))
                next_idx += 1
            if debug_log:
                debug_log.write(f"  No float found for key '{metric_key}' after line {idx+1}\n")
            break
    if debug_log:
        debug_log.write(f"Key '{metric_key}' not found in {simout_path}\n")
    return None

def eval_expr(expr, values):
    # Replace $(...) with the corresponding value from values dict
    def repl(m):
        key = m.group(1)
        if key not in values or values[key] is None:
            return 'None'  # Mark missing values as None for partial evaluation
        return str(values[key])
    expr_eval = re.sub(r'\$\(([^)]+)\)', repl, expr)
    try:
        if 'None' in expr_eval:
            return 'ERR'
        return eval(expr_eval)
    except Exception as e:
        return f"ERR: {e}"



def main():
    if len(sys.argv) != 3:
        print("Usage: python extract_metrics.py <config.json> <metrics.json>")
        sys.exit(1)

    config_path = sys.argv[1]
    metrics_path = sys.argv[2]

    debug_log_path = "extract_metrics_debug.log"
    debug_log = open(debug_log_path, "w")

    with open(config_path) as f:
        config = json.load(f)
    with open(metrics_path) as f:
        metrics = json.load(f)

    build_dir = os.path.abspath(os.path.join(os.path.dirname(config_path), '..', '..', 'build'))

    for benchmark, bench_cfg in config.items():
        simulators = bench_cfg.get('simulators', {})
        for sim, group_list in simulators.items():
            sim_metrics = metrics.get(sim, [])
            # Build headers: if an expression is a tuple, split into multiple columns
            sim_metric_headers = []
            expr_is_tuple = []
            tuple_expr_parts = []
            for expr in sim_metrics:
                if expr.strip().startswith('(') and expr.strip().endswith(')'):
                    inner = expr.strip()[1:-1]
                    parts = [p.strip() for p in inner.split(',')]
                    for part in parts:
                        # Use the part itself as the header, not the full tuple
                        sim_metric_headers.append(part)
                    expr_is_tuple.append(len(parts))
                    tuple_expr_parts.append(parts)
                else:
                    sim_metric_headers.append(expr)
                    expr_is_tuple.append(1)
                    tuple_expr_parts.append(None)
            sim_rows = []
            for group in group_list:
                for cfg_name in group:
                    simout_path = os.path.join(build_dir, benchmark, sim, f"{cfg_name}_simout.res")
                    metric_results = []
                    for expr, ncols, parts in zip(sim_metrics, expr_is_tuple, tuple_expr_parts):
                        keys = re.findall(r'\$\(([^)]+)\)', expr)
                        values = {}
                        for key in keys:
                            val = extract_value_from_simout(simout_path, key, debug_log)
                            values[key] = val
                        try:
                            if ncols > 1 and parts:
                                # Tuple/list: evaluate each part separately, output each in its own column
                                for part in parts:
                                    part_expr = re.sub(r'\$\(([^)]+)\)', lambda m: str(values.get(m.group(1), 'None')), part)
                                    if 'None' in part_expr:
                                        metric_results.append('ERR')
                                    else:
                                        try:
                                            val = eval(part_expr)
                                            # If value is a tuple/list, flatten it (shouldn't happen, but just in case)
                                            if isinstance(val, (tuple, list)):
                                                metric_results.extend(val)
                                            else:
                                                metric_results.append(val)
                                        except Exception as e:
                                            metric_results.append(f"ERR: {e}")
                            else:
                                result = eval_expr(expr, values)
                                # If result is a tuple/list, flatten it (shouldn't happen, but just in case)
                                if isinstance(result, (tuple, list)):
                                    metric_results.extend(result)
                                else:
                                    metric_results.append(result)
                        except Exception as e:
                            metric_results.extend(['ERR'] * (ncols if ncols else 1))
                    sim_rows.append([cfg_name] + metric_results)
            # Write per-sim metrics CSV with descriptive name
            metrics_dir = os.path.join(build_dir, benchmark, sim, "metrics")
            os.makedirs(metrics_dir, exist_ok=True)
            sim_csv_path = os.path.join(metrics_dir, f"{benchmark}_{sim}_metrics.csv")
            with open(sim_csv_path, 'w', newline='') as sim_csv:
                writer = csv.writer(sim_csv)
                writer.writerow(["config"] + sim_metric_headers)
                for sim_row in sim_rows:
                    writer.writerow(sim_row)
            print(f"Wrote: {sim_csv_path}")
    print("Per-simulator metrics CSVs written to metrics/ folders.")
    debug_log.close()

if __name__ == "__main__":
    main()
