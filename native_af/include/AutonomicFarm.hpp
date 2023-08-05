#pragma once

#include "Manager.hpp"
#include "MonitoredFarm.hpp"


namespace native_af
{
	class AutonomicFarm
	{
	private:
		MonitoredFarm farm;
		Monitor monitor;
		Manager manager;


	public:
		explicit AutonomicFarm(unsigned int starting_workers = 1,
					  unsigned int max_workers = std::max(1u, std::thread::hardware_concurrency()),
					  unsigned int monitor_queues_length = 200,
					  double average_time_span = 3.0,
					  double confidence_epsilon = 0.5,
					  unsigned int queue_size_max = 100,
					  unsigned int queue_size_min = 10,
					  double time_to_recover_queue = 10.0,
					  double manager_frequency = 5.0,
					  double wait_after_action = 5.0,
					  MonitorLogger logger = MonitorLogger{});

		void start();
		void stop();

		template<typename Tin, typename Tout>
		unsigned long int add_task(std::function<Tout(Tin)> func, Tin& input)
		{
			return this->farm.add_task(func, input);
		}

		template<typename Tout>
		Tout& get_result()
		{
			return this->farm.get_result<Tout>();
		}
	};
}