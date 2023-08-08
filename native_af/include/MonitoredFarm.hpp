#pragma once

#include <atomic>
#include <thread>
#include <chrono>

#include "Farm.hpp"
#include "MonitorInfo.hpp"


namespace native_af
{
	class Monitor;

	/**
	 * Class that implements a farm that can be monitored by a Monitor.
	 */
	class MonitoredFarm : public Farm
	{
	private:
		SharedQueue<MonitorInfo> monitor_queue;
		std::atomic<bool> log_info = false;
		SharedQueue<std::thread::id> worker_exited_queue;

		std::mutex farm_mutex;

		void worker_func();

	public:
		friend class Manager;
		friend class Monitor;
		friend class Executor;

		/**
		 * Basic MonitoredFarm constructor.
		 * @param n_workers Number of workers.
		 */
		explicit MonitoredFarm(unsigned int n_workers);

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
		template<typename Tin, typename Tout>
		unsigned long int add_task(std::function<Tout(Tin)> func, Tin& input)
		{
			unsigned long int id = Farm::add_task<Tin, Tout>(func, input);
			if(this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_ARRIVED, std::this_thread::get_id(), id,
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}
			return id;
		}

		/**
		 * Get the first result in the output queue of the farm.
		 * @tparam Tout type of the output of the task
		 * @return result of the task
		 *
		 * Pop the first task in the output queue,
		 * push in the monitor queue the information that a task has been taken from the output_queue,
		 * and return the output of the task.
		 */
		template<typename Tout>
		Tout& get_result()
		{
			auto task = this->output_queue.pop().get();
			if(this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_TAKEN, std::this_thread::get_id(), task->get_id(),
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}
			return static_cast<TypedTask<Tout>*>(task)->get_output();
		}

		/**
		 * Start all the workers of the farm.
		 */
		void start();
	};
}