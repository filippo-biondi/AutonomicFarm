#pragma once

#include <chrono>
#include <map>

#include "MonitoredFarm.hpp"
#include "CircularVector.hpp"
#include "MonitorLogger.hpp"

using std::chrono::microseconds;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

namespace native_af
{
	class Monitor
	{
	private:
		MonitoredFarm& farm;
		std::thread monitor_thread;
		std::atomic<bool> stop_thread = false;
		unsigned int queues_length;
		std::mutex monitor_mutex;

		std::map<std::thread::id, high_resolution_clock::time_point> worker_started_map;
		CircularVector<high_resolution_clock::time_point> task_finished_times;
		CircularVector<high_resolution_clock::time_point> task_arrived_times;
		CircularVector<high_resolution_clock::time_point> task_taken_times;
		CircularVector<std::pair<high_resolution_clock::time_point, duration<double>>> workers_latency;

		MonitorLogger logger;
		high_resolution_clock::time_point start_time;

	public:
		Monitor(MonitoredFarm& farm, unsigned int queues_length, MonitorLogger logger);

		duration<double> get_instant_latency();
		duration<double> get_latency(double time_span);
		double get_estimated_worker_overhead(double time_span);

		double get_instant_throughput();
		double get_throughput(double time_span);

		double get_instant_arrival_frequency();
		double get_arrival_frequency(double time_span);

		double get_instant_taken_frequency();
		double get_taken_frequency(double time_span);

		unsigned long int get_arrival_queue_size();
		unsigned int get_n_worker();

		void start();
		void stop();
		void log(double time_span);
		void monitor_func();
	};

}