#pragma once

#include <chrono>
#include <map>

#include "IMonitor.hpp"
#include "MonitoredFarm.hpp"
#include "CircularVector.hpp"
#include "MonitorLogger.hpp"

using std::chrono::microseconds;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

namespace native
{
	/**
	 * Class that implements the monitor of the MAPE loop.
	 */
	class Monitor : public IMonitor
	{
	private:
		std::shared_ptr<MonitoredFarm> farm;
		std::thread monitor_thread;
		std::atomic<bool> stop_thread = false;

		void monitor_func();

	public:
		/**
		 * Basic Monitor constructor.
		 * @param farm the farm on which the monitor operate.
		 * @param vectors_length the length of the circular vectors used to store the farm's metrics.
		 * @param logger the logger used to log the monitor data.
		 */
		Monitor(std::shared_ptr<MonitoredFarm> farm, unsigned int vectors_length, MonitorLogger logger);

		/**
		 * Get the size of the arrival queue of the farm.
		 * @return the arrival queue size.
		 */
		unsigned long int get_arrival_queue_size() override;
		/**
		 * Get the current number of worker in the farm.
		 * @return the number of worker.
		 */
		unsigned int get_n_worker() override;
		/**
		 * Start the monitor thread.
		 */
		void start() override;
		/**
		 * Stop the monitor thread.
		 */
		void stop() override;
	};

}