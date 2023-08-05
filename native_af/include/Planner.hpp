#pragma once

#include <chrono>
#include <algorithm>
#include <cmath>

#include "Action.hpp"


namespace native_af
{
	class Planner
	{
	private:
		unsigned int current_workers;
		unsigned int max_workers;
		double time_to_recover_queue;
		double wait_after_action;
		std::chrono::high_resolution_clock::time_point last_change;

	public:
		Planner(unsigned int current_workers, unsigned int max_workers, double time_to_recover_queue, double wait_after_action);

		int plan(Action action, double throughput, double arrival_frequency, double task_time, unsigned int queue_size);
	};
}