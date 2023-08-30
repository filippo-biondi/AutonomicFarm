#include <pthread.h>
#include <iostream>

#include "native/Farm.hpp"

namespace native
{

	Farm::Farm(unsigned int n_workers) : n_workers{n_workers}, workers{n_workers}, input_queue{}, output_queue{}
	{
	}

	unsigned long int Farm::add_task(ITask* task)
	{
		this->last_id++;
		task->set_id(this->last_id);
		auto task_shared_ptr = std::shared_ptr<ITask>(task);
		this->input_queue.push(task_shared_ptr);
		return last_id;
	}

	std::shared_ptr<ITask> Farm::get_result()
	{
		return this->output_queue.pop();
	}

	/**
	 * Push n. of workers EoSTask in the input_queue and wait for all the workers to exit.
	 * This will wait for all the task that were already present in the queue to be executed.
	 */
	void Farm::stop()
	{
		for (unsigned int i = 0; i < this->n_workers; i++)
		{
			std::shared_ptr<ITask> eos_task = std::make_shared<EoSTask>();
			this->input_queue.push(eos_task);
		}
		for (unsigned int i = 0; i < this->n_workers; i++)
		{
			this->workers[i].join();
		}
	}

	/**
	 * Push n. of workers EoSTask on the front of the input_queue and wait for all the workers to exit.
	 * This will cause the farm to stop faster since the task that are in the queue are not executed.
	 */
	void Farm::fast_stop()
	{
		for (unsigned int i = 0; i < this->n_workers; i++)
		{
			std::shared_ptr<ITask> eos_task = std::make_shared<EoSTask>();
			this->input_queue.push_front(eos_task);
		}
		for (unsigned int i = 0; i < this->n_workers; i++)
		{
			this->workers[i].join();
		}
	}

	/**
	 * Start n. of workers new threads and push them in the workers vector.
	 */
	void Farm::start()
	{
		int i = 0;
		for (auto &worker: this->workers)
		{
			worker = std::thread(&Farm::worker_func, this);
#if PIN_TO_CORES
			std::cout << "Pinning thread " << i << " to core " << i << "\n";
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(i, &cpuset);
			int rc = pthread_setaffinity_np(worker.native_handle(),
			                                sizeof(cpu_set_t), &cpuset);
			if (rc != 0)
			{
				std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
			}
			i++;
#endif
		}
	}

	/**
	 * Pop a task from the input_queue, run it and push it in the output_queue.
	 * Repeat these actions until an EOS task is received.
	 */
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
	}
}