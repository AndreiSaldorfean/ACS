import subprocess
import os
import multiprocessing

script_path = os.path.abspath(os.path.dirname(__file__))
msim_dir = os.path.join(script_path, "..", "m-sim_v2.0")
results_dir = os.path.join(msim_dir, "results")

benchmarks = ["applu", "equake", "galgel", "lucas", "mesa", "mgrid"]

def run_single_group(lvpSize):
    global benchmarks
    for benchmark in benchmarks:
        cmd = f"{msim_dir}/sim-outorder -redir:sim {results_dir}/{benchmark}.res -fastfwd 1000000 -max:inst 10000000 -lvpt:size {lvpSize} {results_dir}/{benchmark}.arg"
        subprocess.run(cmd, shell=True)



if __name__ == "__main__":
    os.chdir(msim_dir)
    with multiprocessing.Pool(2) as pool:
        pool.map(run_single_group, [16, 32])
