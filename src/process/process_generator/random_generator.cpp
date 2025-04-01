#include <chrono>
#include "random_generator.hpp"

// generate gen with seed
RandomGenerator::RandomGenerator() :
	gen(std::chrono::system_clock::now().time_since_epoch().count()) {} // Use system time to generate seed

// exponential distribution ( generate process times of arrival)
double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(gen);
}

// normal distribution using mean and standard deviation
int RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return std::max(1, (int)round(dist(gen))); // rounds to the nearest integer and ensures that the minimum value is 1
}

// uniform distribution between min & max
int RandomGenerator::uniform(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}