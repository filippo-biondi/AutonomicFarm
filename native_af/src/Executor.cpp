#include "Executor.hpp"


namespace native_af
{
	Executor::Executor(MonitoredFarm& farm) : farm{farm}
	{}

	void Executor::add_workers(unsigned int n)
	{
		std::unique_lock<std::mutex> lock(this->farm.farm_mutex);
		for(unsigned int i=0; i < n; i++)
		{
			std::thread new_worker = std::thread(&MonitoredFarm::worker_func, &this->farm);
			this->farm.workers.push_back(std::move(new_worker));
		}

		this->farm.n_workers += n;
	}

	void Executor::remove_workers(unsigned int n)
	{
		std::unique_lock<std::mutex> lock(this->farm.farm_mutex);
		for(unsigned int i=0; i < n; i++)
		{
			auto eos_task = std::shared_ptr<Task>(new EoSTask());
			this->farm.input_queue.push_front(eos_task);
		}

		for(unsigned int i=0; i < n; i++)
		{
			auto id = this->farm.worker_exited_queue.pop();
			for(auto worker= this->farm.workers.begin(); worker != this->farm.workers.end(); worker++)
			{
				if(worker->get_id() == id)
				{
					worker->join();
					this->farm.workers.erase(worker);
					break;
				}
			}
		}
		this->farm.n_workers -= n;
	}
}