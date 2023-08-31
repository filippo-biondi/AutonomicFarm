#include "Planner.hpp"


Planner::Planner(unsigned int current_workers, unsigned int min_workers, unsigned int max_workers, double time_to_recover_queue, double wait_after_action) :
		current_workers{current_workers},
		min_workers{min_workers},
		max_workers{max_workers},
		time_to_recover_queue{time_to_recover_queue},
		wait_after_action{wait_after_action},
		last_change{std::chrono::high_resolution_clock::now()}
{}

int Planner::plan(Action action, double throughput, double arrival_frequency, double task_time, unsigned int queue_size)
{
	double average_worker_service_time = 1.0 / task_time;
	int ideal_workers = static_cast<int>(this->current_workers);
	int new_workers;
	switch(action)
	{
		case Action::INCREASE_WORKER:
			ideal_workers = static_cast<int>(this->current_workers) + std::max(1, static_cast<int>((arrival_frequency - throughput) / average_worker_service_time));
			break;
		case Action::DECREASE_WORKER:
			ideal_workers = static_cast<int>(this->current_workers) - static_cast<int>((throughput - arrival_frequency) / average_worker_service_time);
			break;
		case Action::RECOVER_QUEUE:
			ideal_workers = static_cast<int>(this->current_workers) + static_cast<int>((arrival_frequency +
			                                                          queue_size / this->time_to_recover_queue -
			                                                          throughput) / average_worker_service_time);
			break;
		case Action::NO_ACTION:
			ideal_workers = static_cast<int>(this->current_workers);
			break;
	}

	new_workers = std::clamp(ideal_workers, static_cast<int>(this->min_workers), static_cast<int>(this->max_workers));

	int diff = new_workers - static_cast<int>(this->current_workers);

	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_diff = current_time - this->last_change;
	if(diff != 0)
	{
		if(time_diff.count() < wait_after_action)
		{
			return 0;
		}
		this->last_change = std::chrono::high_resolution_clock::now();
	}

	this->current_workers = static_cast<unsigned int>(new_workers);

	return diff;
}