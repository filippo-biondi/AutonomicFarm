#pragma once

#include <thread>
#include <algorithm>

#include "MonitoredFarm.hpp"
#include "Task.hpp"


namespace native_af
{
	class Executor
	{
	private:
		MonitoredFarm& farm;

	public:
		explicit Executor(MonitoredFarm& farm);

		void add_workers(unsigned int n);

		void remove_workers(unsigned int n);
	};
}