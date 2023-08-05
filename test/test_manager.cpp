#include <iostream>
#include <fstream>

#include "Manager.hpp"
#include "AutonomicFarm.hpp"
#include "MonitorLogger.hpp"


using namespace native_af;


int test_func(int x)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	return x;
}

int main(int argc, char* argv[])
{
	std::ofstream log_file;
	log_file.open("../test_log.txt");

	AutonomicFarm farm{2,
					   10,
					   300,
					   5.0,
					   0.5,
					   100,
					   10,
					   10.0,
					   5.0,
					   2.0,
					   MonitorLogger{{&log_file, &std::cout}}};

	farm.start();

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
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	for (int i = 0; i < 50; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::this_thread::sleep_for(std::chrono::seconds(15));

	farm.stop();

	log_file.close();
	return 0;
}