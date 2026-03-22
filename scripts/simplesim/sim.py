import shutil
import json
import os
import subprocess
import sys
try:
    from tqdm import tqdm
except ImportError:
    tqdm = None

script_path = os.path.abspath(os.path.dirname(__file__))
out_dir_root = os.path.join(script_path, "..", "..", "build")
cmd_log_file = os.path.join(out_dir_root, "cmd_run.log")


def cleanup():

    if os.path.exists(out_dir_root):
        shutil.rmtree(out_dir_root)


def get_input():

    file_name = "config.json"
    file_name = os.path.join(script_path, file_name)
    file_data = []

    try:
        with open(file_name) as file_handle:
            file_data = json.load(file_handle)
    except Exception as e:
        print(f"Cannot open {file_name + "\nException: " + e.__str__()}!")
        sys.exit(1)

    return file_data


def get_commands(benchmark_cfg: dict):

    sim_root = os.path.join(script_path, "..", "..", "simplesim-3.0")
    commands = []

    for benchmark in benchmark_cfg:

        executable_file = benchmark_cfg[benchmark]["executable_file"]
        input_file = benchmark_cfg[benchmark]["input"]
        max_instructions = benchmark_cfg[benchmark]["max_instructions"]

        for sim in benchmark_cfg[benchmark]["simulators"]:
            os.makedirs(out_dir_root + "/" + benchmark + "/" + sim, exist_ok=True)
            benchmark_dir = os.path.join(out_dir_root + "/" + benchmark, sim)
            os.makedirs(benchmark_dir, exist_ok=True)

            for config in benchmark_cfg[benchmark]["simulators"][sim]:

                cfg_value = benchmark_cfg[benchmark]["simulators"][sim][config]
                cfg_name = config
                sim_out = os.path.join(benchmark_dir, f"{cfg_name}_simout.res")
                prog_out = os.path.join(benchmark_dir, f"{cfg_name}_progout.res")
                cmd = f"{sim_root}/sim-{sim} -redir:sim {sim_out} -redir:prog {prog_out} -max:inst {max_instructions} {cfg_value} {executable_file} {input_file}"
                commands.append(cmd)

    return commands


def run_command(command: str):

    try:
        with open(cmd_log_file, "a") as file_handle:

            result = subprocess.run(
                command, shell=True, stdout=file_handle, stderr=file_handle
            )
            file_handle.write(f"Ran {command}\n")
            file_handle.write(f"Exit code {result.returncode}\n")
            file_handle.write("============\n")

    except Exception as e:
        print(f"Cannot run command {command}! Error: {e}")
        sys.exit(1)


def main():

    cleanup()

    benchmark_cfg = get_input()
    commands = get_commands(benchmark_cfg)


    if tqdm is None:
        print("tqdm not installed. Progress bar will not be shown. To enable, install tqdm: pip install tqdm")
        iterator = commands
    else:
        iterator = tqdm(commands, desc="Running simulations", unit="sim")

    for cmd in iterator:
        run_command(cmd)


main()
