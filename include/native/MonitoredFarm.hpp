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
	 * Class that implements a farm that can be monitored by a Monitor.
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
		 * @param n_workers Number of workers.
		 */
		MonitoredFarm(unsigned int n_workers, unsigned int max_workers);

		/**
		 * Add a task to be executed by the farm.
		 * @tparam Tin type of the input of the task
		 * @tparam Tout type of the output of the task
		 * @param func the function to be executed
		 * @param input the input of the function
		 * @return the id of the task
		 *
		 * Create a new task with the given function and the given input (the id of the task will be the smallest unused id starting from 1)
		 * and push it in the input queue.
		 * Push in the monitor queue the information that a task has arrived in the input_queue.
		 */
		unsigned long int add_task(ITask* task) override;

		/**
		 * Get the first result in the output queue of the farm.
		 * @tparam Tout type of the output of the task
		 * @return result of the task
		 *
		 * Pop the first task in the output queue,
		 * push in the monitor queue the information that a task has been taken from the output_queue,
		 * and return the output of the task.
		 */
		std::shared_ptr<ITask> get_result() override;

		/**
		 * Start all the workers of the farm.
		 */
		void start() override;

		void stop() override;
	};
}