#include <utility>

#include "AutonomicFarm.hpp"
#include "fastflow/MonitoredFarm.hpp"
#include "native/Executor.hpp"
#include "native/Monitor.hpp"


/**
 * Create a MonitoredFarm, a Monitor, an Analyzer, a Planner, and an Executor.
 * Create a Manager and pass to it the references to the farm and the monitor and the values af all the other component of the MAPE loop.
 */
AutonomicFarm::AutonomicFarm(bool fastflow,
							 unsigned int starting_workers,
                             unsigned int min_workers,
							 unsigned int max_workers,
                             unsigned int monitor_vectors_length,
                             double average_time_span,
                             double confidence_epsilon,
                             unsigned int queue_upper_limit,
                             unsigned int queue_lower_limit,
                             double time_to_recover_queue,
                             double manager_frequency,
                             double wait_after_action,
                             const MonitorLogger& logger) :
		time_span{average_time_span}, wait_time{1.0 / manager_frequency}
{
	if(fastflow)
	{
		auto fastflow_monitor = std::make_shared<fastflow::Monitor>(monitor_vectors_length, logger);
		auto fastflow_executor = std::make_shared<fastflow::Executor>();
		this->farm = std::make_shared<fastflow::MonitoredFarm>(starting_workers, max_workers, fastflow_monitor, fastflow_executor);
		this->monitor = fastflow_monitor;
		this->executor = fastflow_executor;

	}
	else
	{
		auto native_monitored_farm = std::make_shared<native::MonitoredFarm>(starting_workers, max_workers);

		this->monitor = std::make_shared<native::Monitor>(native_monitored_farm, monitor_vectors_length, logger);
		this->executor = std::make_shared<native::Executor>(native_monitored_farm);
		this->farm = native_monitored_farm;
	}

	this->analyser = std::make_shared<Analyser>(this->monitor, confidence_epsilon, queue_upper_limit, queue_lower_limit, average_time_span);
	this->planner = std::make_shared<Planner>(starting_workers, min_workers, max_workers, time_to_recover_queue, wait_after_action);
}

/**
 * Start the manager and then start the farm.
 */
void AutonomicFarm::start()
{
	this->monitor->start();
	this->manager_thread = std::thread{&AutonomicFarm::manager_func, this};
	this->farm->start();
}

/**
 * Stop the farm and then stop the manager.
 */
void AutonomicFarm::stop()
{
	this->farm->stop();
	this->stop_thread = true;
	this->monitor->stop();
	this->manager_thread.join();
}

void AutonomicFarm::manager_func()
{
	while (true)
	{
		this->monitor->log(this->time_span);
		auto [action, throughput, arrival_frequency, task_time, queue_size] = this->analyser->analyse();
		auto worker_diff = planner->plan(action, throughput, arrival_frequency, task_time, queue_size);
		if(worker_diff > 0)
		{
			this->executor->add_workers(worker_diff);
		}
		else if(worker_diff < 0)
		{
			this->executor->remove_workers(-worker_diff);
		}

		std::this_thread::sleep_for(this->wait_time);

		if(this->stop_thread) { break; }
	}
}