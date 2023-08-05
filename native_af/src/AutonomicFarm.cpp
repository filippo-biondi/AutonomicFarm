#include "AutonomicFarm.hpp"

#include <utility>

namespace native_af
{
	AutonomicFarm::AutonomicFarm(unsigned int starting_workers,
								 unsigned int max_workers,
	                             unsigned int monitor_queues_length,
	                             double average_time_span,
	                             double confidence_epsilon,
	                             unsigned int queue_upper_limit,
	                             unsigned int queue_lower_limit,
	                             double time_to_recover_queue,
	                             double manager_frequency,
	                             double wait_after_action,
	                             MonitorLogger logger) :
			farm{starting_workers},
			monitor{this->farm, monitor_queues_length, std::move(logger)},
			manager{this->farm,
			this->monitor,
			Analyser{this->monitor, confidence_epsilon, queue_upper_limit, queue_lower_limit, average_time_span},
			Planner{starting_workers, max_workers, time_to_recover_queue, wait_after_action},
		    Executor{farm}, manager_frequency, average_time_span}
	{}

	void AutonomicFarm::start()
	{
		this->manager.start();
		this->farm.start();
	}

	void AutonomicFarm::stop()
	{
		this->farm.stop();
		this->manager.stop();
	}
}