import subprocess
import os

script_path = os.path.abspath(os.path.dirname(__file__))
msim_dir = os.path.join(script_path, "..", "m-sim_v2.0")
results_dir = os.path.join(msim_dir, "results")

benchmarks = ["applu", "equake", "galgel", "lucas", "mesa","mgrid"],
groups2 = [
    [benchmarks[0], benchmarks[1]],
    [benchmarks[1], benchmarks[2]],
    [benchmarks[2], benchmarks[3]],
    [benchmarks[3], benchmarks[4]],
]

group3 = [[benchmarks[0],benchmarks[1],benchmarks[2]],[benchmarks[2],benchmarks[3],benchmarks[4]]]
os.chdir(msim_dir)


def run_single_group(lvpSize):
    for benchmark in benchmarks:
        print(f"RUNNING {benchmark}")
        cmd = f"{msim_dir}/sim-outorder -redir:sim {results_dir}/{benchmark}.res -fastfwd 1000000 -max:inst 10000000 -lvpt:size {lvpSize} {results_dir}/{benchmark}.arg"
        status = subprocess.run(cmd, shell=True)


def run_groups_2(lvpSize):
    for group in groups2:
        print(f"RUNNNING {group}")
        cmd = f"{msim_dir}/sim-outorder -redir:sim {results_dir}/{group[0[0]]}_{group[0[1]]}.res -fastfwd 1000000 -max:inst 10000000 -lvpt:size {lvpSize} {results_dir}/{group[0][0]}.arg {group[0][1]}.arg"
        status = subprocess.run(cmd, shell=True)


run_single_group(16)
run_single_group(32)
run_single_group(64)
run_single_group(256)
run_single_group(512)
run_single_group(1024)
run_single_group(2048)
print("HALT")

# groups2 = [[benchmarks[0],benchmarks[1]],benchmarks[1],benchmarks[2]]
