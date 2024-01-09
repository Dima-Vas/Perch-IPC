
#include <iostream>
#include <cmath>
#include <vector>

void calculate_integral_sum_with_shift(double (*Function)(double, double), const std::vector<double> &inputData, double& result);
void calculate_integral_sum(double (*Function)(double, double), const std::vector<double> &inputData, double& result);

void calculate_integral(double (*Function)(double, double), const std::vector<double> &inputDataDouble,
                       const std::vector<int> &inputDataInt, double& absError, double& relError, double& result){
    double start_X=inputDataDouble[0];
    double start_Y=inputDataDouble[1];
    double end_X=inputDataDouble[2];
    double end_Y=inputDataDouble[3];
    int start_dots_X=inputDataInt[0];
    int start_dots_Y=inputDataInt[1];
    size_t max_iter=inputDataInt[2];
    double integral_Sum=0.0;
    double step_X=(end_X-start_X)/start_dots_X;
    double step_Y=(end_Y-start_Y)/start_dots_Y;
    double delta=step_X*step_Y;
    std::vector<double> function_data={step_X, step_Y, start_X, start_Y, end_X, end_Y};
    calculate_integral_sum(Function, function_data, integral_Sum);
    double integral=integral_Sum*delta;
    double new_integral;
    double cal_absError;
    double cal_relError;
    for (size_t i=0; i < max_iter;i++){
        double new_integral_sum=0;
        delta/=4;
        calculate_integral_sum_with_shift(Function, function_data, new_integral_sum);
        new_integral_sum+=integral_Sum;
        new_integral=new_integral_sum*delta;
        cal_absError=integral-new_integral;
        if (cal_absError < 0) {
            cal_absError*=-1;
        }
        cal_relError=cal_absError/new_integral_sum;
        if ((cal_absError <= absError)||(cal_relError <= relError)){
            result=new_integral;
            absError=cal_absError;
            relError=cal_relError;
            return;
        }
        integral=new_integral;
        integral_Sum=new_integral_sum;
        function_data[0]/=2;
        function_data[1]/=2;
    }
    result=new_integral;
    absError=cal_absError;
    relError=cal_relError;
    return;
}

void calculate_integral_sum_with_shift(double (*Function)(double, double), const std::vector<double> &inputData, double& result){
    double step_X=inputData[0];
    double step_Y=inputData[1];
    double start_X=inputData[2];
    double start_Y=inputData[3];
    double end_X=inputData[4];
    double end_Y=inputData[5];
    double shift_X=step_X/2;
    double shift_Y=step_Y/2;
    result=0;
    for (double x=start_X+shift_X; x<=end_X; x+=step_X){
        for (double y=start_Y+shift_Y; y<=end_Y; y+=step_Y){
            result+=Function(x, y);
        }
    }
    for (double x=start_X+shift_X; x<=end_X; x+=step_X){
        for (double y=start_Y; y<=end_Y; y+=step_Y){
            result+=Function(x, y);
        }
    }
    for (double x=start_X; x<=end_X; x+=step_X){
        for (double y=start_Y+shift_Y; y<=end_Y; y+=step_Y){
            result+=Function(x, y);
        }
    }
}

void calculate_integral_sum(double (*Function)(double, double), const std::vector<double> &inputData, double& result){
    double step_X=inputData[0];
    double step_Y=inputData[1];
    double start_X=inputData[2];
    double start_Y=inputData[3];
    double end_X=inputData[4];
    double end_Y=inputData[5];
    result=0;
    for (double x=start_X; x<=end_X; x+=step_X){
        for (double y=start_Y; y<=end_Y; y+=step_Y){
            result+=Function(x, y);
        }
    }
}

double calculate_function1(double x, double y){
    double result=0.002;
    for (int i = -2; i <= 2; i++){
        for (int j=-2; j <= 2; j++){
            result+=1/((5*(i+2)+j+3+std::pow((x-16*j), 6)+std::pow((y-16*i), 6)));
        }
    }
    return 1/result;
}

double calculate_function2(double x, double y){
    double a=20;
    double b=0.2;
    double c=2*M_PI;
    double output = -a*std::pow(M_E, -b*pow((x*x+y*y)/2,0.5))
                    -std::pow(M_E, (std::cos(c*x)+std::cos(c*y))/2)+a+M_E;
    return output;
}

double calculate_function3(double x, double y){
    size_t m=5;
    std::vector<double> a1{1, 2, 1, 1, 5};
    std::vector<double> a2{4, 5, 1, 2, 4};
    std::vector<double> c{2, 1, 4, 7, 2};
    double result=0;
    for (size_t i=1; i<=m; i++){
        result+= c[i]*std::pow(M_E, (-(std::pow(x-a1[i], 2)+std::pow(y-a2[i], 2))/M_PI))*
                 std::cos(M_PI*(std::pow(x-a1[i], 2)+std::pow(y-a2[i], 2)));
    }
    return -result;
}