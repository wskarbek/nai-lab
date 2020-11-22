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

std::vector<double> generate_random_x(int n, int min, int max) {
    using namespace std;

    vector<double> ret;
    random_device device;
    mt19937 generator(device());
    uniform_int_distribution<int> distribution(min, max);

    for(int i = 0; i < n; i++) {
        ret.push_back(distribution(generator));
    }
    return ret;
}

auto best = [](auto function, int n, int iterations = 1000, double minDomain = -10, double maxDomain = 10) {
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
            }
        }
    }
    
    return bestXs;
};

std::map<std::string, std::string> process_args(int argc, char **argv) {
    using namespace std;

    map<string, string> args;
    string argname = "-m";
    args[argname] = "tabu";
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

    int min, max, iterations;

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

    vector<double> bestXs;

    if (args.find("-f") != args.end()) {
        if (args["-f"] == "sphere") {
            bestXs = best(sphere_f, 10);
        }
        if (args["-f"] == "matyas") {
            bestXs = best(matyas_f, 2);
        }
        if (args["-f"] == "rosenbrock") {
            bestXs = best(rosenbrock_f, 10);
        }
    }

    cout << "Best solutions are such X'es: " << endl;
    for (int i = 0; i < bestXs.size(); i++) {
        cout << "x[" << (i+1) << "]: " << bestXs.at(i) << endl; 
    }
    cout << endl;
}