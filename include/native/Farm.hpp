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
	 * Class that implements a basic farm.
	 */
	class Farm
	{
	private:
		unsigned long int last_id = 0;

		/**
		 * The function that each worker executes.
		 */
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
		 * @tparam Tin type of the input of the task
		 * @tparam Tout type of the output of the task
		 * @param func the function to be executed
		 * @param input the input of the function
		 * @return the id of the task
		 *
		 * Create a new task with the given function and the given input (the id of the task will be the smallest unused id starting from 1)
		 * and push it in the input queue.
		 */
		unsigned long int add_task(std::function<void()> func, void* output);

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
		 * Get the first result in the output queue of the farm.
		 * @tparam Tout type of the output of the task
		 * @return result of the task
		 */
		void* get_result();
	};
}