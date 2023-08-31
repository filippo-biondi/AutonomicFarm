#include <iostream>
#include <fstream>

#include "AutonomicFarm.hpp"


unsigned long int divisors(unsigned long int x)
{
	unsigned long int divisors = 0;
	for(unsigned long int i=2; i < x; i++)
	{
		if(x % i == 0)
		{
			divisors++;
		}
	}
	return divisors;
}

int main()
{
	std::ofstream log_file;
	log_file.open("../test_fastflow_latency.txt");

	AutonomicFarm farm{true,
	                   5,
	                   1,
	                   64,
	                   1000,
	                   5.0,
	                   2.0,
	                   100,
	                   10,
	                   10.0,
	                   5.0,
	                   3.0,
	                   MonitorLogger{{&log_file}, true}};

	farm.start();

	unsigned long input = 99997649;
	for (int i = 0; i < 200; i++)
	{
		farm.add_task<unsigned long int, unsigned long int>(divisors, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
	}

	input = 10525143;
	for (int i = 0; i < 200; i++)
	{
		farm.add_task<unsigned long int, unsigned long int>(divisors, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
	}

	input = 469772493;
	for (int i = 0; i < 200; i++)
	{
		farm.add_task<unsigned long int, unsigned long int>(divisors, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
	}

	input = 254540074;
	for (int i = 0; i < 200; i++)
	{
		farm.add_task<unsigned long int, unsigned long int>(divisors, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
	}

	farm.stop();

	for(int i = 0; i < 10; i++)
	{
		auto result = farm.get_result<unsigned long int>();
		std::cout << result << std::endl;
	}

	log_file.close();
	return 0;
}