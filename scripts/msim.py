import subprocess
import os
import multiprocessing

script_path = os.path.abspath(os.path.dirname(__file__))
msim_dir = os.path.join(script_path, "..", "m-sim_v2.0")
results_dir = os.path.join(msim_dir, "results")
out_dir = os.path.join(script_path, "build")
inputs_dir = os.path.join(script_path, "..", "inputs")
benchmarks = ["applu", "equake", "galgel", "lucas", "mesa", "mgrid"]

pairs = [
    [benchmarks[0], benchmarks[1]],
    [benchmarks[2], benchmarks[3]],
    [benchmarks[4], benchmarks[5]],
]

triplets = [
    [benchmarks[0], benchmarks[1], benchmarks[2]],
    [benchmarks[3], benchmarks[4], benchmarks[5]],
]


def run_single_group(lvptSize):

    for benchmark in benchmarks:
        arg_file_name = f"{benchmark}_{lvptSize}.arg"
        arg_file = os.path.join(inputs_dir, "args", benchmark, arg_file_name)
        cmd = f"{msim_dir}/sim-outorder -redir:sim {out_dir}/{benchmark}_{lvptSize}.res -fastfwd 1000000 -max:inst 10000000 -lvpt:size {lvptSize} {arg_file}"
        subprocess.run(cmd, shell=True)


def run_2_group(lvptSize):

    for group in pairs:
        arg_file_name0 = f"{group[0]}_{lvptSize}.arg"
        arg_file_name1 = f"{group[1]}_{lvptSize}.arg"
        arg_file0 = os.path.join(inputs_dir, "args", group[0], arg_file_name0)
        arg_file1 = os.path.join(inputs_dir, "args", group[1], arg_file_name1)
        cmd = f"{msim_dir}/sim-outorder -redir:sim {out_dir}/{group[0]}_{group[1]}_{lvptSize}.res -fastfwd 1000000 -max:inst 10000000 -lvpt:size {lvptSize} {arg_file0} {arg_file1}"

        subprocess.run(cmd, shell=True)


if __name__ == "__main__":

    lvpt_sizes = [16, 32, 64, 128, 256, 512, 1024, 2048]
    os.makedirs(out_dir, exist_ok=True)
    os.chdir(msim_dir)
    # with multiprocessing.Pool(os.cpu_count()) as pool:
    #     pool.map(run_single_group, lvpt_sizes)

    with multiprocessing.Pool(os.cpu_count()) as pool:
        pool.map(run_2_group, lvpt_sizes)
