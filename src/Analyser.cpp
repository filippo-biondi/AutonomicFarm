#include "Analyser.hpp"

#include <utility>



Analyser::Analyser(std::shared_ptr<IMonitor> monitor, double epsilon, unsigned int queue_upper_limit, unsigned int queue_lower_limit, double average_time_span) :
monitor{std::move(monitor)}, epsilon{epsilon}, queue_upper_limit{queue_upper_limit}, queue_lower_limit{queue_lower_limit}, time_span{average_time_span}
{}

/**
 * Get form the Monitor the current throughput, arrival frequency, average task time and queue size and:
 * - If the current arrival frequency is greater than the current throughput,
 * the Analyser will suggest to increase the number of workers and will update the estimated overhead
 * (this is done because a real estimate of the overhead can be measured by the Monitor only when all the threads are working).
 * - If the current arrival frequency is lower than the current throughput, the Analyser will suggest to decrease the number of workers.
 * - If the current arrival frequency is equal to the current throughput,
 * the Analyser will suggest to decrease the number of workers only if the estimated throughput
 * (calculated basing on average time for a task, estimated overhead and number of workers) is greater than the current arrival frequency.
 * In this case the throughput returned is the estimated one
 * - If the queue size is greater than the upper limit, the Analyser will suggest to recover the queue until the queue size will be lower than the lower limit.
 * - Otherwise, the Analyser will suggest to take no action.
 */
std::tuple<Action, double, double, double, unsigned int> Analyser::analyse()
{
	double current_throughput = this->monitor->get_throughput(this->time_span);
	double current_arrival_frequency = this->monitor->get_arrival_frequency(this->time_span);
	double average_task_time = this->monitor->get_latency(this->time_span).count();
	double estimated_throughput = 1.0 / (average_task_time + this->estimated_overhead) * this->monitor->get_n_worker();
	unsigned int current_queue_size = this->monitor->get_arrival_queue_size();

	Action action;

	if(current_throughput < current_arrival_frequency - this->epsilon)
	{
		action = Action::INCREASE_WORKER;
		this->estimated_overhead = this->monitor->get_estimated_worker_overhead(this->time_span);
	}
	else if(current_throughput > current_arrival_frequency + this->epsilon)
	{
		action = Action::DECREASE_WORKER;
	}
	else if(current_throughput > current_arrival_frequency - this->epsilon &&
	        current_throughput < current_arrival_frequency + this->epsilon &&
	        estimated_throughput > current_arrival_frequency + this->epsilon)
	{
		action = Action::DECREASE_WORKER;
		current_throughput = estimated_throughput;
	}
	else
	{
		action = Action::NO_ACTION;
	}

	if(current_queue_size > this->queue_upper_limit ||
	   (this->last_action == Action::RECOVER_QUEUE &&
	    current_queue_size > this->queue_lower_limit))
	{
		action = Action::RECOVER_QUEUE;
	}

	this->last_action = action;

	return {action, current_throughput, current_arrival_frequency, average_task_time, current_queue_size};
}