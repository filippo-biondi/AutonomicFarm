#pragma once

#include <map>
#include <chrono>
#include <ff/ff.hpp>
#include <utility>


#include "Task.hpp"
#include "IMonitor.hpp"
#include "MonitorInfo.hpp"
#include "CircularVector.hpp"
#include "MonitorLogger.hpp"
#include "fastflow/MonitoredFarm.hpp"

using std::chrono::microseconds;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

namespace fastflow
{

	class MonitoredFarm;

	/**
	 * FastFlow implementation of the monitor.
	 */
	class Monitor : public ff::ff_minode, public IMonitor
	{
	private:
		MonitoredFarm* farm_ptr = nullptr;
		bool started = false;
		bool stopped = false;

	public:
		/**
		 * Basic Monitor constructor.
		 * @param vectors_length the length of the circular vector used by the Monitor
		 * @param logger the logger to log monitored metrics
		 */
		Monitor(unsigned int vectors_length, MonitorLogger logger);

		void *svc(void *t) override;

		void eosnotify(ssize_t) override;

		/**
		 * Get the size of the arrival queue (currently always return 0).
		 */
		unsigned long int get_arrival_queue_size() override;

		/**
		 * Get the current number of worker of the monitored farm.
		 */
		unsigned int get_n_worker() override;

		/**
		 * Set the pointer to the farm to be monitored.
		 */
		void set_farm_ptr(MonitoredFarm* ptr);

		/**
		 * Start the execution of the monitor.
		 */
		void start() override;

		/**
		 * Start the execution of the monitor.
		 */
		void stop() override;
	};
}