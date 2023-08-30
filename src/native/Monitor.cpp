#include <iostream>
#include <iomanip>
#include <numeric>
#include <utility>

#include "native/Monitor.hpp"

namespace native
{
	Monitor::Monitor(std::shared_ptr<MonitoredFarm> farm, unsigned int vectors_length, MonitorLogger logger) :
	IMonitor{vectors_length, std::move(logger)},
	farm{std::move(farm)}
	{}

	void Monitor::start()
	{
		this->start_time = high_resolution_clock::now();
		this->monitor_thread = std::thread{&Monitor::monitor_func, this};
		this->farm->log_info = true;
	}

	void Monitor::stop()
	{
		this->stop_thread = true;
		auto item = MonitorInfo{InfoType::TASK_ARRIVED, std::this_thread::get_id(), 0, high_resolution_clock::now()};
		this->farm->monitor_queue.push_front(item);
		this->monitor_thread.join();
	}



	void Monitor::monitor_func()
	{
		while (true)
		{
			if(this->stop_thread) { break; }

			auto info = this->farm->monitor_queue.pop();
			duration<double> latency{};

			{
				std::unique_lock<std::mutex> lock(this->monitor_mutex);
				switch (info.info_type)
				{
					case InfoType::TASK_STARTED:
						this->worker_started_map[info.thread_id] = info.time_point;
						break;

					case InfoType::TASK_FINISHED:
						latency = info.time_point - this->worker_started_map[info.thread_id];
						this->workers_latency.push_back({info.time_point, latency});
						break;

					case InfoType::TASK_PUSHED:
						this->task_pushed_times.push_back(info.time_point);
						break;

					case InfoType::TASK_ARRIVED:
						this->task_arrived_times.push_back(info.time_point);
						break;

					case InfoType::TASK_TAKEN:
						this->task_taken_times.push_back(info.time_point);
						break;
				}
			}
		}
	}

	unsigned long Monitor::get_arrival_queue_size()
	{
		std::unique_lock<std::mutex> lock(this->monitor_mutex);
		return this->farm->input_queue.size();
	}

	unsigned int Monitor::get_n_worker()
	{
		return this->farm->current_workers;
	}
}