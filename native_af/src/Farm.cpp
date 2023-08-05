#include <pthread.h>
#include <iostream>

#include "Farm.hpp"

using namespace native_af;

Farm::Farm(unsigned int n_workers) : n_workers{n_workers}, workers{n_workers}, input_queue{}, output_queue{}
{
}

void Farm::stop()
{
	for(unsigned int i = 0; i < this->n_workers; i++)
	{
		auto eos_task = std::shared_ptr<Task>(new EoSTask());
		this->input_queue.push(eos_task);
	}
	for (unsigned int i = 0; i < this->n_workers; i++)
	{
		this->workers[i].join();
	}
}

void Farm::fast_stop()
{
	for(unsigned int i = 0; i < this->n_workers; i++)
	{
		auto eos_task = std::shared_ptr<Task>(new EoSTask());
		this->input_queue.push_front(eos_task);
	}
	for (unsigned int i = 0; i < this->n_workers; i++)
	{
		this->workers[i].join();
	}
}

void Farm::start()
{
	int i = 0;
	for (auto & worker : this->workers)
	{
		worker = std::thread(&Farm::worker_func, this);
#if PIN_TO_CORES
		std::cout << "Pinning thread " << i << " to core " << i << "\n";
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(i, &cpuset);
		int rc = pthread_setaffinity_np(worker.native_handle(),
		                                sizeof(cpu_set_t), &cpuset);
		if (rc != 0) {
			std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
		}
		i++;
#endif
	}
}

void Farm::worker_func()
{
	while (true)
	{
		auto task = this->input_queue.pop();
		if (task->is_eos())
		{
			break;
		}
		task->run();
		this->output_queue.push(task);
	}
//	int x{0};
//	for (int j = 0; j < 1000; j++)
//	{
//		for(int i = 0; i < 1000000; i++)
//		{
//			x += 1;
//		}
//	}
}