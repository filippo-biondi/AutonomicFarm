#include "AutonomicFarm.hpp"

#include <utility>

namespace native_af
{
	/**
	 * Create a MonitoredFarm, a Monitor, an Analyzer, a Planner, and an Executor.
	 * Create a Manager and pass to it the references to the farm and the monitor and the values af all the other component of the MAPE loop.
	 */
	AutonomicFarm::AutonomicFarm(unsigned int starting_workers,
								 unsigned int max_workers,
	                             unsigned int monitor_vectors_length,
	                             double average_time_span,
	                             double confidence_epsilon,
	                             unsigned int queue_upper_limit,
	                             unsigned int queue_lower_limit,
	                             double time_to_recover_queue,
	                             double manager_frequency,
	                             double wait_after_action,
	                             MonitorLogger logger) :
			farm{starting_workers},
			monitor{this->farm, monitor_vectors_length, std::move(logger)},
			manager{this->farm,
			this->monitor,
			Analyser{this->monitor, confidence_epsilon, queue_upper_limit, queue_lower_limit, average_time_span},
			Planner{starting_workers, max_workers, time_to_recover_queue, wait_after_action},
		    Executor{farm}, manager_frequency, average_time_span}
	{}

	/**
	 * Start the manager and then start the farm.
	 */
	void AutonomicFarm::start()
	{
		this->manager.start();
		this->farm.start();
	}

	/**
	 * Stop the farm and then stop the manager.
	 */
	void AutonomicFarm::stop()
	{
		this->farm.stop();
		this->manager.stop();
	}
}