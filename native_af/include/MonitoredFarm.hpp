#pragma once

#include <atomic>
#include <thread>
#include <chrono>

#include "Farm.hpp"
#include "MonitorInfo.hpp"


namespace native_af
{
	class Monitor;

	class MonitoredFarm : public Farm
	{
	private:
		SharedQueue<MonitorInfo> monitor_queue;
		std::atomic<bool> log_info = false;
		SharedQueue<std::thread::id> worker_exited_queue;

		std::mutex farm_mutex;

		void worker_func();

	public:
		friend class Manager;
		friend class Monitor;
		friend class Executor;

		explicit MonitoredFarm(unsigned int n_workers);

		template<typename Tin, typename Tout>
		unsigned long int add_task(std::function<Tout(Tin)> func, Tin& input)
		{
			unsigned long int id = Farm::add_task<Tin, Tout>(func, input);
			if(this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_ARRIVED, std::this_thread::get_id(), id,
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}
			return id;
		}

		template<typename Tout>
		Tout& get_result()
		{
			auto task = this->output_queue.pop().get();
			if(this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_TAKEN, std::this_thread::get_id(), task->get_id(),
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}
			return static_cast<TypedTask<Tout>*>(task)->get_output();
		}

		void start();
	};
}