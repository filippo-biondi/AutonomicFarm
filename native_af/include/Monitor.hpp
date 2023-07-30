#pragma once

#include <chrono>
#include <map>

#include "MonitoredFarm.hpp"
#include "CircularVector.hpp"

using std::chrono::microseconds;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

namespace native_af
{
	class Monitor
	{
	private:
		MonitoredFarm& farm;
		unsigned int moving_average_length;
		double moving_average_alpha;

		std::map<std::thread::id, high_resolution_clock::time_point> worker_started_map;


		CircularVector<high_resolution_clock::time_point> task_finished_times;
		CircularVector<high_resolution_clock::time_point> task_arrived_times;
		CircularVector<high_resolution_clock::time_point> task_taken_times;
		std::map<std::thread::id, CircularVector<duration<double>>> worker_latency_map;

	public:
		Monitor(MonitoredFarm& farm, unsigned int moving_average_length);

		duration<double> get_instant_latency(unsigned int worker);
		duration<double> get_moving_average_latency(unsigned int worker);

		double get_instant_throughput();
		double get_moving_average_throughput();

		double get_instant_arrival_frequency();
		double get_moving_average_arrival_frequency();

		double get_instant_taken_frequency();
		double get_moving_average_taken_frequency();

		unsigned long int get_arrival_queue_size();

		void start();
		void monitor_func();
	};

}