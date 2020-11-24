#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>
#include <array>
#include <memory>
#include <functional>
#include <set>
#include <list>
#include <map>
#include <chrono>
#include <numeric>
#include <fstream>

void save_plot(std::vector<double> solTimes, std::string name) {
    using namespace std;

    ofstream file(name + ".txt");
    if (file.is_open()) {
        for(int i = 0; i < solTimes.size(); i++) {
            file << (i + 1) << " " << solTimes.at(i) << endl;
        }
        file.close();
    }
}

void save_plot_avg(std::vector<std::vector<double>> solutionTimes, std::string name) {
    using namespace std;

    std::vector<double> solutionTimeAvg;

    cout << solutionTimes.at(0).size();
    save_plot(solutionTimes.at(0), (name + "_test"));
    for (int j = 0; j < solutionTimes.at(0).size(); j++) {
        double avg = 0;
        for(int i = 0; i < solutionTimes.size(); i++) {
            avg += solutionTimes.at(i).at(j);
        }
        avg /= solutionTimes.size();
        cout << "AVG[" << j << "]: " << avg << endl;
        solutionTimeAvg.push_back(avg);
    }
    save_plot(solutionTimeAvg, (name + "_avg"));
}

std::vector<double> generate_random_x(int n, int min, int max) {
    using namespace std;

    vector<double> ret;
    random_device device;
    mt19937 generator(device());
    uniform_real_distribution<double> distribution(min, max);

    for(int i = 0; i < n; i++) {
        ret.push_back(distribution(generator));
    }
    return ret;
}

auto best = [](auto function, std::vector<double> & solutionTime, int n = 10, int iterations = 1000, double minDomain = -100, double maxDomain = 100) {
    using namespace std;

    double min = minDomain;
    double max = maxDomain;

    vector<double> bestXs;

    if (n > 1) {
        double solution = function(generate_random_x(n, minDomain, maxDomain));
        for (int i = 0; i < iterations; i++) {
            vector<double> randomXs = generate_random_x(n, minDomain, maxDomain);
            double newSolution = function(randomXs);
            if (newSolution > solution) {
                bestXs = randomXs;
                solution = newSolution;
            }
            solutionTime.push_back(solution);
        }
    }
    
    return bestXs;
};

std::map<std::string, std::string> process_args(int argc, char **argv) {
    using namespace std;

    map<string, string> args;
    string argname = "";
    for (auto arg : vector<string>(argv + 1, argv + argc)) {
        if (arg.size() && arg[0] == '-') {
            argname = arg;
        } else {
            args[argname] = arg;
        }
    }
    return args;
}

int main(int argc, char **argv) {
    using namespace std;

    map<string, string> args = process_args(argc, argv);

    vector<double> bestXs;

    int iterations = 0, n = 0;//, min = 0, max = 0;
    bool params = false, tests = false;

    //Sphere function
    auto sphere_f = [](vector<double> x) {
        double sum = 0;
        for (auto xi : x) sum += xi * xi;
        return sum;
    };
    
    //Matyas function   
    auto matyas_f = [](vector<double> xi) {
        double x = xi.at(0);
        double y = xi.at(1);
        return 0.26 * ((x * x) + (y * y)) - (0.48 * x * y);
    };

    //Rosenbrock function
    auto rosenbrock_f = [](vector<double> xi) {
        double sum = 0;
        for(int i = 0; i < xi.size() - 1; i++) {
            sum+= (100 * pow((xi.at(i+1) - pow(xi.at(i), 2)), 2) + pow((1 - xi.at(i)), 2));
        }
        return sum;
    };

    if (args.find("-i") != args.end()) iterations = stoi(args["-i"]);
    if (args.find("-n") != args.end()) n = stoi(args["-n"]);
    if (args.find("-t") != args.end()) tests = (bool)stoi(args["-t"]);
    /*if (args.find("-min") != args.end()) min = stoi(args["-min"]);
    if (args.find("-max") != args.end()) max = stoi(args["-max"]);*/

    if (iterations > 0 && n > 1/* && min != 0 && max != 0*/) params = true;

    if (args.find("-f") != args.end()) {
        if (args["-f"] == "sphere") {
            std::vector<double> solutionTime;
            if(tests) {
                std::vector<std::vector<double>> solutionTimes;
                for(int i = 0; i < 20; i++) {
                    bestXs = params ? best(sphere_f, solutionTime, n, iterations/*, min, max*/) : best(sphere_f, solutionTime);
                    solutionTimes.push_back(solutionTime);
                    solutionTime.clear();
                }
                save_plot_avg(solutionTimes, "sphere");
            } else {
                bestXs = params ? best(sphere_f, solutionTime, n, iterations/*, min, max*/) : best(sphere_f, solutionTime);
                save_plot(solutionTime, "sphere");
            }
        }
        if (args["-f"] == "matyas") {
            std::vector<double> solutionTime;
            if (tests) {   
                std::vector<std::vector<double>> solutionTimes;
                for (int i = 0; i < 20; i++) {
                    bestXs = best(matyas_f, solutionTime, 2, 100, -10, 10);
                    solutionTimes.push_back(solutionTime);
                    solutionTime.clear();
                }
                save_plot_avg(solutionTimes, "matyas");
            } else {
                bestXs = best(matyas_f, solutionTime, 2, 100, -10, 10);
                save_plot(solutionTime, "matyas");
            }
        }
        if (args["-f"] == "rosenbrock") {
            std::vector<double> solutionTime;
            if (tests) {
                std::vector<std::vector<double>> solutionTimes;
                for (int i = 0; i < 20; i++) {
                    bestXs = params ? best(rosenbrock_f, solutionTime, n, iterations/*, min, max*/) : best(rosenbrock_f, solutionTime);
                    solutionTimes.push_back(solutionTime);
                }
                save_plot_avg(solutionTimes, "rosenbrock");
            } else {
                bestXs = params ? best(rosenbrock_f, solutionTime, n, iterations/*, min, max*/) : best(rosenbrock_f, solutionTime);
                save_plot(solutionTime, "rosenbrock");
            }
        }
    }

    cout << "Best solutions are such X'es: " << endl;
    for (int i = 0; i < bestXs.size(); i++) {
        cout << "x[" << (i+1) << "]: " << bestXs.at(i) << endl; 
    }
    cout << endl;
}