#include <iostream>
#include <thread>
#include <fstream>
#include "get_time.hpp"
#include <vector>
#include "func_cfg_reader.hpp"
#include <map>
#include "integrate_serial.hpp"
#include <algorithm>

#include <unistd.h>
#include <sys/wait.h>
#include <PIPC/api/SharedMemory.h>

enum ExitCode{
    Success=0,
    WrongArgsNum=1,
    WrongFunctionNum=2,
    CouldNotOpenConfig=3,
    ConfigFileError=5,
    NotEnoughAccuracy=16
};

struct arguments {
    std::vector<double> input_data_double;
    std::vector<int> input_data_int;
    std::vector<double> abs_error;
    std::vector<double> rel_error;
};
int calculate_IPC(double (*Function)(double, double), int process_amount, arguments& args);

int main(int argc, char* argv[]){
    std::string file_name;
    char method;
    int thread_amount;
    if (argc!=4){
        return WrongArgsNum;
    }
    try{
        file_name = argv[1];
        method = *argv[2];
        thread_amount = std::stoi(argv[3]);
    }
    catch (const std::exception&){
        return WrongArgsNum;
    }
    std::ifstream File;
    try
    {
        File.open(file_name);
    }
    catch (const std::exception&)
    {
        return CouldNotOpenConfig;
    }
    if(!File) {
        std::cerr << "Error: file could not be opened" << std::endl;
        return ConfigFileError;
    }
    std::map<std::string, double> configmap;
    read_arguments(File, configmap);
    File.close();
    double (*Function)(double, double);
    switch (method) {
        case '1':
            Function=calculate_function1;
            break;
        case '2':
            Function=calculate_function2;
            break;
        case '3':
            Function=calculate_function3;
            break;
        default:
            return WrongFunctionNum;

    }
    std::vector<double> input_data_double={configmap["x_start"], configmap["y_start"], configmap["x_end"], configmap["y_end"]};

    std::vector<double> input_data_not_int={configmap["init_step_x"], configmap["init_step_y"], configmap["max_iter"]};
    std::vector<int> input_data_int(input_data_not_int.begin(), input_data_not_int.end());

    std::vector<double> abs_error(thread_amount ,configmap["abs_err"]);
    std::vector<double> rel_error(thread_amount, configmap["rel_err"]) ;

    arguments args {input_data_double, input_data_int, abs_error, rel_error};

    int res= calculate_IPC(Function, thread_amount, args);
    return res;
}

int calculate_IPC(double (*Function)(double, double), int process_amount, arguments& args) {
    SharedMemory<double> results("new_shmem_obj", process_amount);

    std::vector<std::vector<double>> regions;
    std::vector<double> curr_region;
    double region_size_x = (std::abs(args.input_data_double[0])+std::abs(args.input_data_double[2]))/process_amount;
    for (double i = args.input_data_double[0]; i < args.input_data_double[2]; i+=region_size_x) {
        curr_region.push_back(i);
        curr_region.push_back(args.input_data_double[1]);
        curr_region.push_back(i+region_size_x);
        curr_region.push_back(args.input_data_double[3]);
        regions.push_back(curr_region);
        curr_region.clear();
    }

    auto start = get_current_time_fenced();
    for (size_t i = 0; i < regions.size(); ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            double local_result = 0.0;
            calculate_integral(Function, regions[i], args.input_data_int, args.abs_error[i], args.rel_error[i], local_result);

            results.write(local_result, i);

            exit(EXIT_SUCCESS);
        } else if (pid < 0) {
            std::cerr << "Fork failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    int status;
    for (size_t i = 0; i < regions.size(); ++i) {
        wait(&status);
    }

    double total_res = 0.0;
    for (size_t i = 0; i < regions.size(); ++i) {
        total_res += results.read(i);
    }

    double rel_error = *std::max_element(args.rel_error.begin(), args.rel_error.end());
    double abs_error = *std::max_element(args.abs_error.begin(), args.abs_error.end());
    long int total_time = to_mi_sec(get_current_time_fenced() - start);

    std::cout << total_res << "\n" << rel_error <<"\n"<< abs_error<< "\n" << total_time << std::endl;

    return total_res;
}