#pragma once

#include <chrono>
#include <algorithm>
#include <cmath>

#include "Action.hpp"


namespace native_af
{
	/**
	 * Class that implements the planner of the MAPE loop.
	 */
	class Planner
	{
	private:
		unsigned int current_workers;
		unsigned int max_workers;
		double time_to_recover_queue;
		double wait_after_action;
		std::chrono::high_resolution_clock::time_point last_change;

	public:
		/**
		 * Basic Planner constructor.
		 * @param current_workers the number of workers currently in the farm.
		 * @param max_workers the maximum number of workers that can be in the farm.
		 * @param time_to_recover_queue the ideal time needed to bring back the queue size to 0.
		 * @param wait_after_action minimum time to wait after an action is taken before taking another one.
		 */
		Planner(unsigned int current_workers, unsigned int max_workers, double time_to_recover_queue, double wait_after_action);

		/**
		 * Decide how to vary the number of workers of the farm based on the information provided by the Analyzer.
		 * @param action the action provided by the Analyzer.
		 * @param throughput the throughput of the farm.
		 * @param arrival_frequency the arrival frequency of the farm.
		 * @param task_time the average time needed to execute a task.
		 * @param queue_size the current size of the queue.
		 * @return the difference between the new number of worker and the old one.
		 */
		int plan(Action action, double throughput, double arrival_frequency, double task_time, unsigned int queue_size);
	};
}