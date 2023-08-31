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

	void Farm::start()
	{
		for (auto &worker: this->workers)
		{
			worker = std::thread(&Farm::worker_func, this);
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
	}
}