#pragma once


namespace native_af
{
	enum InfoType
	{
		TASK_ARRIVED,
		TASK_STARTED,
		TASK_FINISHED,
		TASK_TAKEN,
	};

	struct MonitorInfo
	{
		InfoType info_type;
		std::thread::id thread_id;
		unsigned long int task_id;
		std::chrono::high_resolution_clock::time_point time_point;
	};
}