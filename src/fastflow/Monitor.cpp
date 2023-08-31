#include "fastflow/Monitor.hpp"

namespace fastflow
{

	Monitor::Monitor(unsigned int vectors_length, MonitorLogger logger) :
			IMonitor{vectors_length, std::move(logger)}
	{}

	void* Monitor::svc(void *t)
	{

		if (fromInput())
		{
			this->task_arrived_times.push_back(high_resolution_clock::now());
			return t;
		}
		else
		{
			auto *info = static_cast<MonitorInfo *>(t);

			if(!started || stopped)
			{
				delete info;
				return GO_ON;
			}

			duration<double> latency{};

			{
				std::unique_lock<std::mutex> lock(this->monitor_mutex);
				switch (info->info_type)
				{
					case InfoType::TASK_STARTED:
						this->worker_started_map[info->thread_id] = info->time_point;
						break;

					case InfoType::TASK_FINISHED:
						latency = info->time_point - this->worker_started_map[info->thread_id];
						this->workers_latency.push_back({info->time_point, latency});
						break;

					case InfoType::TASK_PUSHED:
						this->task_pushed_times.push_back(info->time_point);
						break;

					default:
						throw std::runtime_error("Monitor info not recognized");
				}
			}
			delete info;
		}
		return GO_ON;
	}

	void Monitor::eosnotify(ssize_t)
	{
		ff_send_out(EOS);
	}

	unsigned long int Monitor::get_arrival_queue_size()
	{
		std::unique_lock<std::mutex> lock(this->monitor_mutex);
		return this->farm_ptr->input_queue.size();
	}

	unsigned int Monitor::get_n_worker()
	{
		return this->farm_ptr->n_workers;
	}

	void Monitor::set_farm_ptr(MonitoredFarm* ptr)
	{
		this->farm_ptr = ptr;
	}

	void Monitor::start()
	{
		if(!this->farm_ptr)
		{
			throw std::runtime_error("Monitor not set up correctly");
		}
		this->start_time = high_resolution_clock::now();
		started = true;
	}

	void Monitor::stop()
	{
		stopped = true;
	}
}