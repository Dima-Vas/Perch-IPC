import subprocess

def run_cpp_program():
    # Specify the path to your C++ program
    cpp_program_path = 'cmake/build/my_boost_process'

    # Run the C++ program
    process = subprocess.Popen([cpp_program_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output, error = process.communicate()

    # Return the output for further processing
    return output

def main():
    success_count = 0
    failure_count = 0

    # Run the C++ program a million times
    for _ in range(10000):
        print(_)
        output = run_cpp_program()

        # Extract the last line from the output
        last_line = output.strip().split('\n')[-1]

        # Check if the last printed value is 5
        if last_line.split(" ")[-1] == "5":
            success_count += 1
        else:
            failure_count += 1

    # Print the results
    print(f"Success count: {success_count}")
    print(f"Failure count: {failure_count}")

if __name__ == "__main__":
    main()