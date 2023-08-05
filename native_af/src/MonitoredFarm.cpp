#include "MonitoredFarm.hpp"
#include "Monitor.hpp"


using namespace native_af;

MonitoredFarm::MonitoredFarm(unsigned int n_workers) : Farm{n_workers}, monitor_queue{}
{
}

void MonitoredFarm::start()
{
	for (auto & worker : this->workers)
	{
		worker = std::thread(&MonitoredFarm::worker_func, this);
	}
}

void MonitoredFarm::worker_func()
{
	while (true)
	{
		auto task = this->input_queue.pop();
		if(this->log_info)
		{
			MonitorInfo time_info = {InfoType::TASK_STARTED, std::this_thread::get_id(), task->get_id(),
			                         std::chrono::high_resolution_clock::now()};
			this->monitor_queue.push(time_info);
		}
		if (task->is_eos())
		{
			break;
		}
		task->run();
		this->output_queue.push(task);
		if(this->log_info)
		{
			MonitorInfo time_info = {InfoType::TASK_FINISHED, std::this_thread::get_id(), task->get_id(),
			             std::chrono::high_resolution_clock::now()};
			this->monitor_queue.push(time_info);
		}
	}
	auto my_id = std::this_thread::get_id();
	this->worker_exited_queue.push(my_id);
}
