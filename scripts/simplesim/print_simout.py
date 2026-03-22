import json
import os
import sys

# Usage: python print_simout.py config.json

def main():
    if len(sys.argv) != 2:
        print("Usage: python print_simout.py <config.json>")
        sys.exit(1)

    config_path = sys.argv[1]
    if not os.path.exists(config_path):
        print(f"Config file {config_path} does not exist.")
        sys.exit(1)

    with open(config_path) as f:
        config = json.load(f)

    build_dir = os.path.abspath(os.path.join(os.path.dirname(config_path), '..', '..', 'build'))

    for benchmark, bench_cfg in config.items():
        simulators = bench_cfg.get('simulators', {})
        for sim, group_list in simulators.items():
            print(f"\n=== Benchmark: {benchmark} | Simulator: {sim} ===")
            for group in group_list:
                for cfg_name in group:
                    simout_path = os.path.join(build_dir, benchmark, sim, f"{cfg_name}_simout.res")
                    print(f"\n--- {simout_path} ---")
                    if os.path.exists(simout_path):
                        print(f"Contents of {simout_path}:")
                        # with open(simout_path) as simout_file:
                        #     print(simout_file.read())
                    else:
                        print(f"File not found: {simout_path}")

if __name__ == "__main__":
    main()
