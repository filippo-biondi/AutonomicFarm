#pragma once

#include <memory>
#include <queue>
#include <thread>
#include <functional>

#include "SharedQueue.hpp"
#include "Task.hpp"

namespace native
{
	/**
	 * Native implementation of a basic farm.
	 */
	class Farm
	{
	private:
		unsigned long int last_id = 0;

		void worker_func();

	protected:
		/**
		 * The number of workers in the farm.
		 */
		unsigned int n_workers;

		/**
		 * Vector containing the thread object of the workers of the farm.
		 */
		std::vector<std::thread> workers;

		/**
		 * Queue containing the tasks to be executed.
		 */
		SharedQueue<std::shared_ptr<ITask>> input_queue;

		/**
		 * Queue containing the tasks that have been executed.
		 */
		SharedQueue<std::shared_ptr<ITask>> output_queue;

	public:
		/**
		 * Basic MonitoredFarm constructor.
		 * @param n_workers the number of workers in the farm
		 */
		explicit Farm(unsigned int n_workers);

		/**
		 * Add a task to be executed by the farm.
		 * @param task pointer to the task to be executed
		 * @return the id of the task
		 */
		unsigned long int add_task(ITask* task);

		/**
		 * Start all the workers of the farm.
		 */
		void start();

		/**
		 * Stop all the workers of the farm waiting for the termination of all the task in the input queue.
		 */
		void stop();

		/**
		 * Stop all the workers of the farm without waiting for the termination of all the task in the input queue.
		 */
		void fast_stop();

		/**
		 * Get the first task in the output queue of the farm.
		 * @return the pointer of the task
		 */
		std::shared_ptr<ITask> get_result();
	};
}