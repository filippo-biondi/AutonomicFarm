#include <iostream>
#include <chrono>

#include "Farm.hpp"
#include "MonitoredFarm.hpp"
#include "Monitor.hpp"

using namespace native_af;

int test_func(int x)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	return x;
}


int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Usage: ./test1 <n_workers>" << std::endl;
		return 1;
	}
	native_af::MonitoredFarm farm{static_cast<unsigned int>(std::stoul(argv[1]))};

	Monitor monitor{farm, 100};
	monitor.start();
	farm.start();

	for (int i = 0; i < 10000000; i++)
	{
		int input = 0;
		farm.add_task<int, int>(test_func, input);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	farm.stop();

	return 0;
}