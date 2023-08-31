#pragma once

#include "IMonitoredFarm.hpp"
#include "IMonitor.hpp"
#include "Analyser.hpp"
#include "Planner.hpp"
#include "IExecutor.hpp"


/**
 * Class that implements the AutonomicFarm.
 */
class AutonomicFarm
{
private:
	std::shared_ptr<IMonitoredFarm> farm;
	std::shared_ptr<IMonitor> monitor;
	std::shared_ptr<Analyser> analyser;
	std::shared_ptr<Planner> planner;
	std::shared_ptr<IExecutor> executor;
	std::thread manager_thread;
	std::chrono::duration<double> time_span;
	std::chrono::duration<double> wait_time;
	std::atomic<bool> stop_thread = false;

	void manager_func();


public:
	/**
	 * Basic AutonomicFarm constructor
	 * @param starting_workers number of starting workers of the farm
	 * @param min_workers minimum number of workers of the farm
	 * @param max_workers maximum number of workers of the farm
	 * @param monitor_vectors_length maximum length of the vectors where monitor saves metrics of the farm
	 * @param average_time_span time span of the moving average used by the monitor
	 * @param confidence_epsilon minimum difference between arrival frequency and farm throughput to cause an action
	 * @param queue_size_max size of the queue of the farm after which an action will be taken in order to reduce the queue size
	 * @param queue_size_min size of the queue after which action to reduce queue size are ceased
	 * @param time_to_recover_queue ideal time to bring back queue size to 0
	 * @param manager_frequency frequency at which the manager run the MAPE loop
	 * @param wait_after_action minimum time to wait after an action is taken before taking another one
	 * @param logger logger used by the monitor to log metrics
	 */
	explicit AutonomicFarm(bool fastflow = true,
						   unsigned int starting_workers = 1,
						   unsigned int min_workers = 1,
						   unsigned int max_workers = std::max(1u, std::thread::hardware_concurrency()),
						   unsigned int monitor_vectors_length = 200,
						   double average_time_span = 3.0,
						   double confidence_epsilon = 0.5,
						   unsigned int queue_size_max = 100,
						   unsigned int queue_size_min = 10,
						   double time_to_recover_queue = 10.0,
						   double manager_frequency = 5.0,
						   double wait_after_action = 5.0,
						   const MonitorLogger& logger = MonitorLogger{});

	/**
	 * Start the AutonomicFarm.
	 */
	void start();
	/**
	 * Stop the AutonomicFarm. This method will stop the MonitoredFarm, the monitor and the MAPE loop.
	 * Stopping the farm will wait for all the tasks to be completed.
	 */
	void stop();

	/**
	 * Add a task in input to the farm.
	 * @tparam Tin type of the input of the task
	 * @tparam Tout type of the output of the task
	 * @param func function to be executed by the task
	 * @param input input of the task
	 * @return id of the task
	 */
	template<typename Tin, typename Tout>
	unsigned long int add_task(std::function<Tout(Tin)> func, Tin& input)
	{
		ITask* task = new Task<Tout>(0, std::bind(func, input));
		return this->farm->add_task(task);
	}

	/**
	 * Get the first result in the output queue of the farm.
	 * @tparam Tout type of the output of the task
	 * @return result of the task
	 */
	template<typename Tout>
	Tout get_result()
	{
		auto task = this->farm->get_result();
		auto task_t = std::static_pointer_cast<Task<Tout>>(task);
		return std::move(task_t->get_output());
	}
};