#pragma once
#include <random>
#include <vector>
class RandomGenerator
{
private:
	std::mt19937 gen; // Mersenne Twister pseudorandom number generator
public:
	RandomGenerator();
	int poisson(double mean);
	double exponential(double lambda);		// generate numbers with exponential dritribution
	int normal(double mean, double stddev); // generate numbers with normal dritribution
	int uniform(int min, int max);
	int weighted(const std::vector<double>& weights);
};