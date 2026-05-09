import os
import sys
import concurrent.futures
import re


def extract_ipc(file_path):
    # Extract only the file name from file path
    file_name = os.path.basename(file_path)

    # Parse the file name to extract benchmarks and LVPT size
    # Format: benchmark0-benchmark1_LVPTSize.res
    # Benchmarks separated by "-", then "_LVPTSize.res"
    parts = file_name.split("_")
    if len(parts) < 2:
        return None  # Invalid format

    benchmarks_part = parts[0]
    lvpt_size_part = parts[1].split(".")[0]  # Remove .res

    # Extract benchmarks (can be 1,2,3, or 5 separated by "-")
    benchmarks = benchmarks_part.split("-")

    # Extract LVPT size
    try:
        lvpt_size = int(lvpt_size_part)
    except ValueError:
        return None  # Invalid LVPT size

    # Read the file and extract the IPC metric
    try:
        with open(file_path, "r") as f_handle:
            content = f_handle.read()

        # Search for IPC metric (assuming format like "IPC = 1.234" or "IPC: 1.234")
        match = re.search(r"IPC\s*[:=]\s*([0-9.]+)", content)
        if match:
            ipc = float(match.group(1))
        else:
            ipc = None
    except (IOError, ValueError):
        ipc = None

    # Return extracted information
    return {
        "file_name": file_name,
        "benchmarks": benchmarks,
        "lvpt_size": lvpt_size,
        "ipc": ipc,
    }


def main():
    script_path = os.path.abspath(os.path.dirname(__file__))
    sim_results_dir = os.path.join(script_path, "build")

    if not os.path.exists(sim_results_dir):
        print("Build not present!")
        sys.exit(1)

    sim_results_files = [f for f in os.listdir(sim_results_dir) if f.endswith(".res")]

    results = []
    with concurrent.futures.ThreadPoolExecutor() as executor:
        futures = [
            executor.submit(extract_ipc, os.path.join(sim_results_dir, f))
            for f in sim_results_files
        ]
        for future in concurrent.futures.as_completed(futures):
            result = future.result()
            if result:
                results.append(result)

    results.sort(key=lambda item: item["file_name"])

    results_file = os.path.join(script_path, "ipc_results.txt")
    with open(results_file, "w") as f:
        for result in results:
            f.write(
                f"File: {result['file_name']}, Benchmarks: {result['benchmarks']}, LVPT Size: {result['lvpt_size']}, IPC: {result['ipc']}\n"
            )


if __name__ == "__main__":
    main()
