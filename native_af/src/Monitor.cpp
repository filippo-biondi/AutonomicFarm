#include <iostream>
#include <iomanip>
#include <numeric>

#include "Monitor.hpp"

namespace native_af
{
	Monitor::Monitor(MonitoredFarm &farm, unsigned int moving_average_length) :
			farm{farm}, moving_average_length{moving_average_length}, moving_average_alpha{1.0 / moving_average_length},
			worker_started_map{}, task_finished_times{moving_average_length}, task_arrived_times{moving_average_length},
			task_taken_times{moving_average_length}, worker_latency_map{}
	{
	}

	void Monitor::start()
	{
		std::thread monitor_thread(&Monitor::monitor_func, this);
		monitor_thread.detach();
		this->farm.log_info = true;
	}

	void Monitor::monitor_func()
	{
		while (true)
		{
			auto info = this->farm.monitor_queue.pop();

			if (info.task_id == 0)
			{
				break;
			}

			if (info.info_type == InfoType::TASK_STARTED)
			{
				this->worker_started_map[info.thread_id] = info.time_point;
			} else if (info.info_type == InfoType::TASK_FINISHED)
			{
				this->task_finished_times.push_back(info.time_point);

				auto latency = info.time_point - this->worker_started_map[info.thread_id];

				if (this->worker_latency_map.find(info.thread_id) == this->worker_latency_map.end())
				{
					this->worker_latency_map.insert(
							{info.thread_id, CircularVector<std::chrono::duration<double>>{this->moving_average_length}});
				}
				this->worker_latency_map.at(info.thread_id).push_back(latency);
			} else if (info.info_type == InfoType::TASK_ARRIVED)
			{
				this->task_arrived_times.push_back(info.time_point);
			} else if (info.info_type == InfoType::TASK_TAKEN)
			{
				this->task_taken_times.push_back(info.time_point);
			}


			if (!this->task_finished_times.empty() && !this->task_arrived_times.empty())
			{
				auto instant_throughput = this->get_instant_throughput();
				auto average_throughput = this->get_moving_average_throughput();

				auto instant_arrival_frequency = this->get_instant_arrival_frequency();
				auto average_arrival_frequency = this->get_moving_average_arrival_frequency();

				std::cout << '\r'
				          << "Instant throughput: " << std::setw(6) << std::setprecision(2) << std::fixed
				          << instant_throughput << " task/s"
				          << "\t Moving average throughput: " << std::setw(5) << std::setprecision(2) << std::fixed
				          << average_throughput << " task/s"
				          << "\t Instant arrival frequency: " << std::setw(5) << std::setprecision(2) << std::fixed
				          << instant_arrival_frequency << " task/s"
				          << "\t Moving average arrival frequency: " << std::setw(5) << std::setprecision(2) << std::fixed
				          << average_arrival_frequency << " task/s"
				          << "\t Queue size: " << std::setw(8) << this->get_arrival_queue_size()
				          //			          << "\t Instant taken frequency: " << this->task_taken_frequency_queue.back()
				          //			          << " tasks per second"
				          //			          << "\t Moving average taken frequency: " << this->average_task_taken_frequency
				          //			          << " tasks per second"
				          << std::flush;
//
//				for(unsigned int i=0; i < this->farm.n_workers; i++)
//				{
//					std::cout << "Worker " << i << ": " << std::setprecision(3) << this->get_instant_latency(i).count() << "\t";
//				}
//				std::cout << "\r" << std::flush;
			}
		}
	}

	duration<double> Monitor::get_instant_latency(unsigned int worker)
	{
		if (worker >= this->farm.n_workers)
		{
			throw std::runtime_error("Worker index out of range");
		}


		std::thread::id thread_id = this->farm.workers[worker].get_id();

		if(this->worker_latency_map.find(thread_id) == this->worker_latency_map.end() ||
		   this->worker_latency_map.at(thread_id).empty())
		{
			return duration<double>{0};
		}

		return this->worker_latency_map.at(thread_id).back();
	}

	duration<double> Monitor::get_moving_average_latency(unsigned int worker)
	{
		if (worker >= this->worker_latency_map.size())
		{
			throw std::runtime_error("Worker index out of range");
		}

		std::thread::id thread_id = this->farm.workers[worker].get_id();

		if(this->worker_latency_map.at(thread_id).empty())
		{
			return duration<double>{0};
		}

		return std::accumulate(this->worker_latency_map.at(thread_id).begin(),
		                       this->worker_latency_map.at(thread_id).end(),
		                       duration<double>{0}) / this->moving_average_length;
	}

	double Monitor::get_instant_throughput()
	{
		if (this->task_finished_times.size() < 2)
		{
			return 0;
		}

		return 1.0 / std::chrono::duration<double>(this->task_finished_times.back() -
		                                           this->task_finished_times.end()[-2]).count();
	}

	double Monitor::get_moving_average_throughput()
	{
		if (this->task_finished_times.size() < 2)
		{
			return 0;
		}

		return static_cast<double >(this->task_finished_times.size() - 1) /
		       std::chrono::duration<double>(this->task_finished_times.back() -
		                                     this->task_finished_times.front()).count();
	}

	double Monitor::get_instant_arrival_frequency()
	{
		if (this->task_arrived_times.size() < 2)
		{
			return 0;
		}

		return 1.0 / std::chrono::duration<double>(this->task_arrived_times.back() -
		                                           this->task_arrived_times.end()[-2]).count();
	}

	double Monitor::get_moving_average_arrival_frequency()
	{
		if(this->task_arrived_times.size() < 2)
		{
			return 0;
		}

		return static_cast<double >(this->task_arrived_times.size() - 1) /
		       std::chrono::duration<double>(this->task_arrived_times.back() -
		                                     this->task_arrived_times.front()).count();
	}

	double Monitor::get_instant_taken_frequency()
	{
		if(this->task_taken_times.size() < 2)
		{
			return 0;
		}

		return 1.0 / std::chrono::duration<double>(this->task_taken_times.back() -
		                                           this->task_taken_times.end()[-2]).count();
	}

	double Monitor::get_moving_average_taken_frequency()
	{
		if(this->task_taken_times.size() < 2)
		{
			return 0;
		}

		return static_cast<double >(this->task_taken_times.size() - 1) /
		       std::chrono::duration<double>(this->task_taken_times.back() -
		                                     this->task_taken_times.front()).count();
	}

	unsigned long Monitor::get_arrival_queue_size()
	{
		return this->farm.input_queue.size();
	}
}