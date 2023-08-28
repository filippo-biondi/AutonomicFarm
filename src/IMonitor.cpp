#include <iomanip>
#include <numeric>
#include <algorithm>

#include "IMonitor.hpp"



IMonitor::IMonitor(unsigned int vectors_length, MonitorLogger logger) :
		worker_started_map{}, task_pushed_times{vectors_length},
		task_arrived_times{vectors_length}, task_taken_times{vectors_length},
		workers_latency{vectors_length}, logger{std::move(logger)}
{}

void IMonitor::log(std::chrono::duration<double> time_span)
{
	auto instant_throughput = this->get_instant_throughput();
	auto average_throughput = this->get_throughput(time_span);

	auto average_worker_latency = this->get_latency(time_span).count();

	auto instant_arrival_frequency = this->get_instant_arrival_frequency();
	auto average_arrival_frequency = this->get_arrival_frequency(time_span);
	auto queue_size = static_cast<double>(this->get_arrival_queue_size());
	auto n_worker = this->get_n_worker();
	auto estimated_overhead = this->get_estimated_worker_overhead(time_span);

	double timestamp = duration<double>(high_resolution_clock::now() - this->start_time).count();
	this->logger.log({{"Timestamp", timestamp},
	                  {"Instant throughput", instant_throughput},
	                  {"Instant arrival frequency", instant_arrival_frequency},
	                  {"Average throughput", average_throughput},
	                  {"Average arrival frequency", average_arrival_frequency},
	                  {"Queue size", queue_size},
	                  {"n. workers", n_worker},
	                  {"Average worker_latency", average_worker_latency},
	                  {"Estimated overhead", estimated_overhead}});
}

duration<double> IMonitor::get_instant_latency()
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	if(this->workers_latency.empty())
	{
		return {};
	}

	return this->workers_latency.back().second;
}

duration<double> IMonitor::get_latency(duration<double> time_span)
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	if(this->workers_latency.size() < 2)
	{
		return {};
	}

	auto front_time = high_resolution_clock::now() - time_span;

	auto front = std::lower_bound(this->workers_latency.begin(),
	                              this->workers_latency.end(),
	                              front_time,
	                              [](auto& pair, auto& time_point)
	                              {
		                              return pair.first < time_point;
	                              });
	if(front == this->workers_latency.end())
	{
		return {};
	}

	long n_task;

	n_task = std::distance(front, this->workers_latency.end());

	return std::accumulate(front,
	                       this->workers_latency.end(),
	                       duration<double>{},
	                       [](auto& sum, auto& pair)
	                       {
		                       return sum + pair.second;
	                       }) / n_task;
}

double IMonitor::get_estimated_worker_overhead(duration<double> time_span)
{
	auto task_latency = this->get_latency(time_span).count();
	auto worker_throughput = this->get_throughput(time_span) / this->get_n_worker();

	if(task_latency == 0 || worker_throughput == 0)
	{
		return 0;
	}

	auto worker_service_time = (1.0 / worker_throughput);
	return std::max(0.0, worker_service_time - task_latency);
}

double IMonitor::get_instant_throughput()
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	if (this->task_pushed_times.size() < 2)
	{
		return 0;
	}

	return 1.0 / std::chrono::duration<double>(this->task_pushed_times.back() -
	                                           this->task_pushed_times.end()[-2]).count();
}

double IMonitor::get_avg(CircularVector<high_resolution_clock::time_point>& vector, duration<double> time_span)
{
	if(vector.size() < 2)
	{
		return 0;
	}

	auto front_time = high_resolution_clock::now() - time_span;

	auto front = std::lower_bound(vector.begin(),
	                              vector.end(),
	                              front_time);
	if(front == vector.end())
	{
		return 0;
	}

	if(front == vector.begin())
	{
		time_span = duration<double>(vector.back() - vector.front());
	}

	long n_task;

	n_task = std::distance(front, vector.end());

	return static_cast<double>(n_task) / time_span.count();
}

double IMonitor::get_throughput(duration<double> time_span)
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	return IMonitor::get_avg(this->task_pushed_times, time_span);
}

double IMonitor::get_instant_arrival_frequency()
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	if (this->task_arrived_times.size() < 2)
	{
		return 0;
	}

	return 1.0 / std::chrono::duration<double>(this->task_arrived_times.back() -
	                                           this->task_arrived_times.end()[-2]).count();
}

double IMonitor::get_arrival_frequency(duration<double> time_span)
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	return IMonitor::get_avg(this->task_arrived_times, time_span);
}

double IMonitor::get_instant_taken_frequency()
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	if(this->task_taken_times.size() < 2)
	{
		return 0;
	}

	return 1.0 / std::chrono::duration<double>(this->task_taken_times.back() -
	                                           this->task_taken_times.end()[-2]).count();
}

double IMonitor::get_taken_frequency(duration<double> time_span)
{
	std::unique_lock<std::mutex> lock(this->monitor_mutex);
	return IMonitor::get_avg(this->task_taken_times, time_span);
}