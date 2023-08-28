#pragma once

#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

#include "MonitorInfo.hpp"


/**
 * Class that implements a farm that can be monitored by a Monitor.
 */
class IMonitoredFarm
{
public:
	virtual ~IMonitoredFarm() = default;

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

	virtual unsigned long int add_task(std::function<void()> func, void* output) = 0;

	/**
	 * Get the first result in the output queue of the farm.
	 * @tparam Tout type of the output of the task
	 * @return result of the task
	 *
	 * Pop the first task in the output queue,
	 * push in the monitor queue the information that a task has been taken from the output_queue,
	 * and return the output of the task.
	 */
	virtual void* get_result() = 0;

	/**
	 * Start all the workers of the farm.
	 */
	virtual void start() = 0;

	virtual void stop() = 0;
};