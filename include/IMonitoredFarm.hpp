#pragma once

#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

#include "MonitorInfo.hpp"
#include "Task.hpp"


/**
 * Interface of the monitored farm.
 */
class IMonitoredFarm
{
public:
	virtual ~IMonitoredFarm() = default;

	virtual unsigned long int add_task(ITask* task) = 0;

	virtual std::shared_ptr<ITask> get_result() = 0;

	virtual void start() = 0;

	virtual void stop() = 0;
};