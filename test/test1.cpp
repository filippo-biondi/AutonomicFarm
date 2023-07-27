#include <iostream>
#include <chrono>

#include "Farm.hpp"

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
	native_af::Farm farm{8};
	unsigned long int t_base = 0;
	unsigned long int t_farm = 0;

	for (int i = 0; i < 1000; i++)
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		int test = test_func(0);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		t_base += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		int input = 0;
		start = std::chrono::steady_clock::now();
		farm.add_task<int, int>(test_func, input);
		int result = farm.get_result<int>();
		end = std::chrono::steady_clock::now();
		t_farm += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}
	farm.stop();

	std::cout << "Base: " << t_base / 10 << std::endl;
	std::cout << "Farm: " << t_farm / 10 << std::endl;

	return 0;
}