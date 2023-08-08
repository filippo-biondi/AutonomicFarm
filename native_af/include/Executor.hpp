#pragma once

#include <thread>
#include <algorithm>

#include "MonitoredFarm.hpp"
#include "Task.hpp"


namespace native_af
{
	/**
	 * Class that implements the Executor component of the MAPE loop.
	 * Executor is responsible for adding and removing workers from the farm.
	 */
	class Executor
	{
	private:
		MonitoredFarm& farm;

	public:
		/**
		 * Basic Executor constructor.
		 * @param farm the farm on which the executor operate
		 */
		explicit Executor(MonitoredFarm& farm);

		/**
		 * Add n workers to the farm.
		 * @param n number of workers to add
		 */
		void add_workers(unsigned int n);

		/**
		 * Remove n workers from the farm.
		 * @param n number of workers to remove
		 */
		void remove_workers(unsigned int n);
	};
}