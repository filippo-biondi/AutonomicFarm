#include <iostream>
#include <chrono>

#include "Farm.hpp"
#include "MonitoredFarm.hpp"


int test_func(int x)
{
	for(int i = 0; i < 1000000; i++)
	{
		x += 1;
	}
	return x;
}


int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Usage: ./test1 <n_workers>" << std::endl;
		return 1;
	}
	native_af::Farm farm{static_cast<unsigned int>(std::stoul(argv[1]))};

	unsigned long int t_base = 0;
	unsigned long int t_farm = 0;

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < 1000; i++)
	{
		int test = test_func(0);

	}
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	t_base = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

	std::cout << "Base: " << t_base / 10 << std::endl;

	for (int i = 0; i < 1000; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
	}

	start = std::chrono::high_resolution_clock ::now();
	farm.start();
	farm.stop();
	end = std::chrono::high_resolution_clock ::now();
	t_farm = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();


	std::cout << "Farm: " << t_farm / 10 << std::endl;

	std::cout << "Speedup: " << (double)t_base / (double)t_farm << std::endl;

	return 0;
}