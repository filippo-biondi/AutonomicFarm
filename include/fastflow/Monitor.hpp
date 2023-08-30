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

	class Monitor : public ff::ff_minode, public IMonitor
	{
	private:
		MonitoredFarm* farm_ptr = nullptr;
		bool started = false;
		bool stopped = false;

	public:
		Monitor(unsigned int vectors_length, MonitorLogger logger);

		void *svc(void *t) override;

		void eosnotify(ssize_t) override;

		unsigned long int get_arrival_queue_size() override;

		unsigned int get_n_worker() override;

		void set_farm_ptr(MonitoredFarm* ptr);

		void start() override;

		void stop() override;
	};
}