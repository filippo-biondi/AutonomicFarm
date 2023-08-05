#pragma once

#include <utility>

#include "MonitoredFarm.hpp"
#include "Monitor.hpp"
#include "Analyser.hpp"
#include "Planner.hpp"
#include "Executor.hpp"


namespace native_af
{
	class Manager
	{
	private:
		MonitoredFarm &farm;
		Monitor& monitor;
		Analyser analyser;
		Planner planner;
		Executor executor;

		std::thread manager_thread;

		double time_span;
		std::chrono::duration<double> wait_time;
		std::atomic<bool> stop_thread = false;

		void manager_func();

	public:


		Manager(MonitoredFarm &farm, Monitor& monitor, Analyser analyser, Planner planner, Executor executor, double frequency, double time_span);

		void start();
		void stop();
	};
}