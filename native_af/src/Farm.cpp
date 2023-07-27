#include "Farm.hpp"
#include "Task.hpp"

using namespace native_af;

Farm::Farm(unsigned int n_workers) : n_workers{n_workers}, workers{n_workers}, input_queue{}, output_queue{}
{
	for (unsigned int i = 0; i < n_workers; i++)
	{
		this->workers[i] = std::thread(&Farm::worker_func, this);
	}
}

void Farm::add_task(std::shared_ptr<Task> task)
{
	this->input_queue.push(task);
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