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

std::vector<double> solutionTime;

void save_plot(std::string name) {
    using namespace std;

    ofstream file(name + ".txt");
    if (file.is_open()) {
        for(int i = 0; i < solutionTime.size(); i++) {
            file << (i + 1) << " " << solutionTime.at(i) << endl;
        }
        file.close();
    }
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

auto best = [](auto function, int n = 10, int iterations = 1000, double minDomain = -100, double maxDomain = 100) {
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
    bool params = false;

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
    /*if (args.find("-min") != args.end()) min = stoi(args["-min"]);
    if (args.find("-max") != args.end()) max = stoi(args["-max"]);*/

    if (iterations > 0 && n > 1/* && min != 0 && max != 0*/) params = true;

    if (args.find("-f") != args.end()) {
        if (args["-f"] == "sphere") {
            bestXs = params ? best(sphere_f, n, iterations/*, min, max*/) : best(sphere_f);
            save_plot("sphere");
        }
        if (args["-f"] == "matyas") {
            bestXs = best(matyas_f, 2, 100, -10, 10);
            save_plot("matyas");
        }
        if (args["-f"] == "rosenbrock") {
            bestXs = params ? best(rosenbrock_f, n, iterations/*, min, max*/) : best(rosenbrock_f);
            save_plot("rosenbrock");
        }
    }

    cout << "Best solutions are such X'es: " << endl;
    for (int i = 0; i < bestXs.size(); i++) {
        cout << "x[" << (i+1) << "]: " << bestXs.at(i) << endl; 
    }
    cout << endl;
}