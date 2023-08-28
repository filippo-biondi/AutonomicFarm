#pragma once

#include <map>
#include <chrono>
#include <ff/ff.hpp>
#include <utility>


#include "native/Monitor.hpp"
#include "Task.hpp"
#include "MonitorInfo.hpp"
#include "CircularVector.hpp"
#include "MonitorLogger.hpp"

using std::chrono::microseconds;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

namespace fastflow
{
	class Monitor : public ff::ff_minode, public IMonitor
	{
	private:
		std::shared_ptr<unsigned int> n_workers_ptr;
		bool started = false;
		bool stopped = false;

	public:
		Monitor(unsigned int vectors_length, MonitorLogger logger) :
				IMonitor{vectors_length, std::move(logger)}
		{}

		void *svc(void *t) override
		{

			if (fromInput())
			{
				auto *task = static_cast<Task *>(t);
				this->task_arrived_times.push_back(high_resolution_clock::now());
				return task;
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
				delete info;
			}
			return GO_ON;
		}

		void eosnotify(ssize_t) override
		{
			ff_send_out(EOS);
		}

//		void svc_end() override
//		{
//			std::cout << "Monitor exited" << std::endl;
//		}

		unsigned long int get_arrival_queue_size() override
		{
			// didn't find a way to access input buffer of the emitter so arrival queue size is not implemented
			return 0;
		}

		unsigned int get_n_worker() override
		{
			return *this->n_workers_ptr;
		}

		void set_n_worker_ptr(std::shared_ptr<unsigned int>& ptr)
		{
			this->n_workers_ptr = ptr;
		}

		void start() override
		{
			if(!this->n_workers_ptr)
			{
				throw std::runtime_error("Monitor not set up correctly");
			}
			started = true;
		}

		void stop() override
		{
			stopped = true;
		}
	};
}