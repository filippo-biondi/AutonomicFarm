#include "native/Executor.hpp"

#include <utility>


namespace native
{
	Executor::Executor(std::shared_ptr<MonitoredFarm> farm) : farm{std::move(farm)}
	{}

	void Executor::add_workers(unsigned int n)
	{
		for(unsigned int i=0; i < n; i++)
		{
			auto a = true;
			this->farm->sleep_queue.push(a);
			this->farm->current_workers++;
		}
	}

	void Executor::remove_workers(unsigned int n)
	{
		for(unsigned int i=0; i < n; i++)
		{
			std::shared_ptr<ITask> sleep_task = std::make_shared<SleepTask>(this->farm->sleep_queue);
			this->farm->input_queue.push_front(sleep_task);
			this->farm->current_workers--;
		}
	}
}