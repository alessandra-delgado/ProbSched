#pragma once
#include <random>

class RandomGenerator
{
	private:
	std::mt19937 gen; // Mersenne Twister pseudorandom number generator
	public:
	RandomGenerator();
	double exponential(double lambda);		// generate numbers with exponential dritribution
	int normal(double mean, double stddev); // generate numbers with normal dritribution
	int uniform(int min, int max);
};