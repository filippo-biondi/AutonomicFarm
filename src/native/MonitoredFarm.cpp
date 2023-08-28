#include "native/MonitoredFarm.hpp"
#include "native/Monitor.hpp"
#include "Task.hpp"

namespace native
{
	MonitoredFarm::MonitoredFarm(unsigned int n_workers, unsigned int max_workers) :
		Farm{max_workers}, current_workers{n_workers}, monitor_queue{}, worker_exited_queue{}
	{}

	unsigned long int MonitoredFarm::add_task(std::function<void()> func, void* output)
	{
		unsigned long int id = Farm::add_task(func, output);
		if(this->log_info)
		{
			MonitorInfo time_info = {InfoType::TASK_ARRIVED, std::this_thread::get_id(), id,
			                         std::chrono::high_resolution_clock::now()};
			this->monitor_queue.push(time_info);
		}
		return id;
	}

	void* MonitoredFarm::get_result()
	{
		auto task = this->output_queue.pop().get();
		if(this->log_info)
		{
			MonitorInfo time_info = {InfoType::TASK_TAKEN, std::this_thread::get_id(), task->get_id(),
			                         std::chrono::high_resolution_clock::now()};
			this->monitor_queue.push(time_info);
		}
		return dynamic_cast<Task*>(task)->get_output();
	}

	/**
	 * Start n. of workers new threads and push them in the workers vector.
	 */
	void MonitoredFarm::start()
	{
		for(unsigned int i=this->workers.size(); i > this->current_workers; i--)
		{
			std::shared_ptr<ITask> sleep_task = std::make_shared<SleepTask>(this->sleep_queue);
			this->input_queue.push(sleep_task);
		}

		for(auto &worker: this->workers)
		{
			worker = std::thread(&MonitoredFarm::worker_func, this);
		}
	}

	void MonitoredFarm::stop()
	{
		for(unsigned int i = this->current_workers; i < this->workers.size(); i++)
		{
			auto a = false;
			this->sleep_queue.push(a);
		}
		Farm::stop();
	}

	/**
	 * Repeat the following actions until an EOS task is received:
	 *   - pop a task from the input_queue,
	 *   - push in the monitor queue the information that a task has started,
	 *   - run the task,
	 *   - push in the monitor queue the information that a task has finished,
	 *   - push the task in the output_queue,
	 *   - push in the monitor queue the information that a task has been pushed in the output_queue.
	 *
	 * After an EOS task is received, push the id of the current thread in the worker_exited_queue.
	 */
	void MonitoredFarm::worker_func()
	{
		while (true)
		{
			auto task = this->input_queue.pop();

			if (task->is_eos())
			{
				break;
			}

			if(task->is_sleep())
			{
				task->run();
				continue;
			}

			if (this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_STARTED, std::this_thread::get_id(), task->get_id(),
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}

			task->run();

			if (this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_FINISHED, std::this_thread::get_id(), task->get_id(),
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}

			this->output_queue.push(task);

			if (this->log_info)
			{
				MonitorInfo time_info = {InfoType::TASK_PUSHED, std::this_thread::get_id(), task->get_id(),
				                         std::chrono::high_resolution_clock::now()};
				this->monitor_queue.push(time_info);
			}
		}
		auto my_id = std::this_thread::get_id();
		this->worker_exited_queue.push(my_id);
	}
}