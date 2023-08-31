#pragma once

#include <atomic>
#include <thread>
#include <chrono>

#include "IMonitoredFarm.hpp"
#include "Farm.hpp"
#include "MonitorInfo.hpp"


namespace native
{
	/**
	 * Native implementation of a farm that can be monitored and with variable number of workers.
	 */
	class MonitoredFarm : public IMonitoredFarm, public Farm
	{
	private:
		std::atomic<unsigned int> current_workers;
		SharedQueue<MonitorInfo> monitor_queue;
		std::atomic<bool> log_info = false;
		SharedQueue<bool> sleep_queue;

		void worker_func();

	public:
		friend class Monitor;
		friend class Executor;

		/**
		 * Basic MonitoredFarm constructor.
		 * @param n_workers starting number of workers.
		 * @param max_workers maximum number of workers.
		 */
		MonitoredFarm(unsigned int n_workers, unsigned int max_workers);

		/**
		 * Add a task to be executed by the farm.
		 * @param task pointer to the task to be executed
		 * @return the id of the task
		 */
		unsigned long int add_task(ITask* task) override;

		/**
		 * Get the first task in the output queue of the farm.
		 * @return the pointer to the task
		 */
		std::shared_ptr<ITask> get_result() override;

		/**
		 * Start all the workers of the farm.
		 */
		void start() override;

		/**
		 * Stop all the workers of the farm.
		 */
		void stop() override;
	};
}