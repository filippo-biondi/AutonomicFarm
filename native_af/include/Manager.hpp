#pragma once

#include <utility>

#include "MonitoredFarm.hpp"
#include "Monitor.hpp"
#include "Analyser.hpp"
#include "Planner.hpp"
#include "Executor.hpp"


namespace native_af
{
	/**
	 * Class that implements the Manager that run the MAPE loop.
	 */
	class Manager
	{
	private:
		MonitoredFarm &farm;
		Monitor& monitor;
		Analyser analyser;
		Planner planner;
		Executor executor;
		std::thread manager_thread;
		std::chrono::duration<double> time_span;
		std::chrono::duration<double> wait_time;
		std::atomic<bool> stop_thread = false;

		/**
		 * Method that run the MAPE loop.
		 */
		void manager_func();

	public:
		/**
		 * Basic Manager constructor.
		 * @param farm Farm on which the manager operate
		 * @param monitor Monitor of the MAPE loop
		 * @param analyser Analyser of the MAPE loop
		 * @param planner Planner of the MAPE loop
		 * @param executor Executor of the MAPE loop
		 * @param frequency Frequency at which the MAPE loop is executed
		 * @param time_span Time span of the moving average retrieved from the monitor and used by the analyzer
		 */
		Manager(MonitoredFarm &farm, Monitor& monitor, Analyser analyser, Planner planner, Executor executor, double frequency, double time_span);

		/**
		 * Start the MAPE loop.
		 */
		void start();
		/**
		 * Stop the MAPE loop.
		 */
		void stop();
	};
}