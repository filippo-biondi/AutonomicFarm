#include <iostream>
#include <fstream>

#include "AutonomicFarm.hpp"


int test_func(double x)
{
	int a = 0;
	auto start_time = high_resolution_clock::now();
	while(duration<double>(high_resolution_clock::now() - start_time).count() < x)
	{
		a++;
	}
	return a;
}

int main()
{
	std::ofstream log_file;
	log_file.open("../test_fastflow_latency.txt");

	AutonomicFarm farm{true,
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

	double input = 0.5;
	for (int i = 0; i < 100; i++)
	{
		farm.add_task<double, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	input = 0.8;
	for (int i = 0; i < 150; i++)
	{
		farm.add_task<double, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	input = 1.2;
	for (int i = 0; i < 100; i++)
	{
		farm.add_task<double, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	input = 0.2;
	for (int i = 0; i < 100; i++)
	{
		farm.add_task<double, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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