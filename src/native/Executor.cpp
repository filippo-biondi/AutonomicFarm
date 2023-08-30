#include "native/Executor.hpp"

#include <utility>


namespace native
{
	Executor::Executor(std::shared_ptr<MonitoredFarm> farm) : farm{std::move(farm)}
	{}

	/**
	 * Start n new threads and push them in the workers vector of the farm.
	 */
	void Executor::add_workers(unsigned int n)
	{
		for(unsigned int i=0; i < n; i++)
		{
			auto a = true;
			this->farm->sleep_queue.push(a);
			this->farm->current_workers++;
		}
	}

	/**
	 * Push n EoSTask in the input_queue of the farm and wait for the workers to exit.
	 * Then join the threads of the exited worker and remove them from the workers vector of the farm.
	 */
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