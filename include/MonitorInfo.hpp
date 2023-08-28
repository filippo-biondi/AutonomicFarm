#pragma once

#include <thread>
#include <chrono>

/**
 * Enum that represents the possible types of information that the Monitor can receive.
 */
enum InfoType
{
	TASK_ARRIVED,
	TASK_STARTED,
	TASK_FINISHED,
	TASK_PUSHED,
	TASK_TAKEN,
};

/**
 * Struct that represents the information that the Monitor can receive from the MonitoredFarm.
 */
struct MonitorInfo
{
	/**
	 * Type of information.
	 */
	InfoType info_type;
	/**
	 * Id of the thread that sent the information.
	 */
	std::thread::id thread_id;
	/**
	 * Id of the task that which the information is about.
	 */
	unsigned long int task_id;
	/**
	 * Timestamp of the information.
	 */
	std::chrono::high_resolution_clock::time_point time_point;
};