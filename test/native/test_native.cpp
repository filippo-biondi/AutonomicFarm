#include <iostream>
#include <fstream>

#include "AutonomicFarm.hpp"


int test_func(int x)
{
	auto start_time = high_resolution_clock::now();
	while(duration<double>(high_resolution_clock::now() - start_time).count() < 0.5)
	{
		x++;
	}
	return x;
}

int main()
{
	std::ofstream log_file;
	log_file.open("../test_native.txt");

	AutonomicFarm farm{false,
	                   5,
	                   2,
	                   10,
	                   1000,
	                   2.0,
	                   0.5,
	                   100,
	                   10,
	                   10.0,
	                   5.0,
	                   2.0,
	                   MonitorLogger{{&log_file}, true}};

	farm.start();

	for (int i = 0; i < 100; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	for (int i = 0; i < 200; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	for (int i = 0; i < 100; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	for (int i = 0; i < 100; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
	}

	farm.stop();

	for(int i = 0; i < 10; i++)
	{
		int result = farm.get_result<int>();
		std::cout << result << std::endl;
	}

	log_file.close();
	return 0;
}