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
	/**
	 * Class that implements the monitor of the MAPE loop.
	 */
	class Monitor
	{
	private:
		MonitoredFarm& farm;
		std::thread monitor_thread;
		std::atomic<bool> stop_thread = false;
		std::mutex monitor_mutex;

		std::map<std::thread::id, high_resolution_clock::time_point> worker_started_map;
		CircularVector<high_resolution_clock::time_point> task_pushed_times;
		CircularVector<high_resolution_clock::time_point> task_arrived_times;
		CircularVector<high_resolution_clock::time_point> task_taken_times;
		CircularVector<std::pair<high_resolution_clock::time_point, duration<double>>> workers_latency;

		MonitorLogger logger;
		high_resolution_clock::time_point start_time;

		static double get_avg(CircularVector<high_resolution_clock::time_point>& vector, duration<double> time_span);
		void monitor_func();

	public:
		/**
		 * Basic Monitor constructor.
		 * @param farm the farm on which the monitor operate.
		 * @param vectors_length the length of the circular vectors used to store the farm's metrics.
		 * @param logger the logger used to log the monitor data.
		 */
		Monitor(MonitoredFarm& farm, unsigned int vectors_length, MonitorLogger logger);

		/**
		 * Get the last recorded latency of the farm.
		 * @return the instant latency of the farm.
		 */
		duration<double> get_instant_latency();
		/**
		 * Get the average latency of the farm.
		 * @param time_span the time span on which the average latency is computed.
		 * @return the average latency of the farm.
		 */
		duration<double> get_latency(duration<double> time_span);
		/**
		 * Get the estimated overhead of each worker relative to the synchronization of shared queues.
		 * @param time_span the time span on which the estimate is computed.
		 * @return the estimated worker overhead.
		 */
		double get_estimated_worker_overhead(duration<double> time_span);
		/**
		 * Get the last recorded throughput of the farm.
		 * @return the instant throughput of the farm.
		 */
		double get_instant_throughput();
		/**
		 * Get the average throughput of the farm.
		 * @param time_span the time span on which the average throughput is computed.
		 * @return the average throughput of the farm.
		 */
		double get_throughput(duration<double> time_span);
		/**
		 * Get the last recorded arrival frequency of the farm.
		 * @return the instant arrival frequency of the farm.
		 */
		double get_instant_arrival_frequency();
		/**
		 * Get the average arrival frequency of the farm.
		 * @param time_span the time span on which the average arrival frequency is computed.
		 * @return the average arrival frequency of the farm.
		 */
		double get_arrival_frequency(duration<double> time_span);
		/**
		 * Get the last recorded taken frequency of the farm.
		 * @return the instant taken frequency of the farm.
		 */
		double get_instant_taken_frequency();
		/**
		 * Get the average taken frequency of the farm.
		 * @param time_span the time span on which the average taken frequency is computed.
		 * @return the average taken frequency of the farm.
		 */
		double get_taken_frequency(duration<double> time_span);
		/**
		 * Get the size of the arrival queue of the farm.
		 * @return the arrival queue size.
		 */
		unsigned long int get_arrival_queue_size();
		/**
		 * Get the current number of worker in the farm.
		 * @return the number of worker.
		 */
		unsigned int get_n_worker();

		/**
		 * Start the monitor thread.
		 */
		void start();
		/**
		 * Stop the monitor thread.
		 */
		void stop();
		/**
		 * Log the monitor data using the MonitorLogger.
		 * @param time_span the time span on which the averages are computed.
		 */
		void log(duration<double> time_span);
	};

}