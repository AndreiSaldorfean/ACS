import json
import os
import subprocess
import sys

script_path = os.path.abspath(os.path.dirname(__file__))


def get_input():

    file_name = "config.json"
    file_name = os.path.join(script_path, file_name)
    file_data = []

    try:
        with open(file_name) as file_handle:
            file_data = json.load(file_handle)
    except:
        print(f"Cannot open {file_name}!")
        sys.exit(1)

    return file_data


def get_commands(benchmark_cfg: dict):

    sim_root = os.path.join(script_path, "..", "..", "simplesim-3.0")
    out_dir_root = os.path.join(script_path, "..", "..", "build")
    commands = []

    os.makedirs(out_dir_root, exist_ok=True)

    for benchmark in benchmark_cfg:

        benchmark_dir = os.path.join(out_dir_root, benchmark)
        os.makedirs(benchmark_dir, exist_ok=True)

        for sim in benchmark_cfg[benchmark]:

            cmd = f"{sim_root}/sim-{sim}"
            for config in benchmark_cfg[benchmark][sim]:

                cfg_value = benchmark_cfg[benchmark][sim][config]
                cfg_name = f"{benchmark}_{sim}_{config}"
                sim_out = os.path.join(benchmark_dir, cfg_name, "simout.res")
                prog_out = os.path.join(benchmark_dir, cfg_name, "progout.res")

                cmd = f"{cmd} -redir:sim {sim_out} -redir:prog {prog_out} {cfg_value}"
                commands.append(cmd)

    return commands


def run_command(command: str):

    try:
        result = subprocess.run(command,shell=True)
    except:
        print(f"Cannot run command {command}!")
        sys.exit(1)

def main():

    benchmark_cfg = get_input()
    commands = get_commands(benchmark_cfg)

main()
