import json
import os

benchmark_cfg = []

def get_input():

    file_name = "config.json"
    file_name = os.path.join(os.getcwd(),file_name)
    file_data = []

    try:
        with open(file_name) as file_handle:
            file_data = json.loads(file_handle)
    except:
        print(f"Cannot open {file_name}!")

    return file_data

def get_command():
    pass

def run_command():
    pass

def main():
    benchmark_cfg = get_input()

main()