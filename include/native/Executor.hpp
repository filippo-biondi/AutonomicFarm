#pragma once

#include <thread>
#include <algorithm>

#include "IExecutor.hpp"
#include "native/MonitoredFarm.hpp"
#include "Task.hpp"


namespace native
{
	/**
	 * Native implementation of the Executor.
	 * Executor is responsible for adding and removing workers from the farm.
	 */
	class Executor : public IExecutor
	{
	private:
		std::shared_ptr<MonitoredFarm> farm;

	public:
		/**
		 * Basic Executor constructor.
		 * @param farm the farm on which the executor operate
		 */
		explicit Executor(std::shared_ptr<MonitoredFarm> farm);

		/**
		 * Add n workers to the farm.
		 * @param n number of workers to add
		 */
		void add_workers(unsigned int n) override;

		/**
		 * Remove n workers from the farm.
		 * @param n number of workers to remove
		 */
		void remove_workers(unsigned int n) override;
	};
}