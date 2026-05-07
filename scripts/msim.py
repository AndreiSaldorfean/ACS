import subprocess
import os
import multiprocessing

script_path = os.path.abspath(os.path.dirname(__file__))
msim_dir = os.path.join(script_path, "..", "m-sim_v2.0")
results_dir = os.path.join(msim_dir, "results")
out_dir = os.path.join(script_path, "build")
inputs_dir = os.path.join(script_path, "..", "inputs")
# benchmarks = ["applu", "equake", "galgel", "lucas", "mesa", "mgrid"]
benchmarks = ["applu"]


def run_single_group(lvptSize):
    global benchmarks
    for benchmark in benchmarks:
        arg_file_name = f"{benchmark}_{lvptSize}.arg"
        arg_file = os.path.join(inputs_dir, "args", benchmark, arg_file_name)
        cmd = f"{msim_dir}/sim-outorder -redir:sim {out_dir}/{benchmark}_{lvptSize}.res -fastfwd 1000000 -max:inst 10000000 -lvpt:size {lvptSize} {arg_file}"
        subprocess.run(cmd, shell=True)


if __name__ == "__main__":

    lvpt_sizes = [16,32,64,128,256,512,1024,2048]
    os.makedirs(out_dir, exist_ok=True)
    os.chdir(msim_dir)
    with multiprocessing.Pool(2) as pool:
        pool.map(run_single_group, lvpt_sizes)
