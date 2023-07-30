#pragma once

#include <memory>
#include <queue>
#include <thread>

#include "SharedQueue.hpp"
#include "TypedTask.hpp"

namespace native_af
{
	class Farm
	{
	private:
		unsigned long int last_id = 0;

		void worker_func();

	protected:
		unsigned int n_workers;
		std::vector<std::thread> workers;
		SharedQueue<std::shared_ptr<Task>> input_queue;
		SharedQueue<std::shared_ptr<Task>> output_queue;

	public:
		explicit Farm(unsigned int n_workers);

		template<typename Tin, typename Tout>
		unsigned long int add_task(std::function<Tout(Tin)> func, Tin& input)
		{
			this->last_id++;
			auto task = std::shared_ptr<Task>(new TypedTask<Tout>(last_id, func, input));
			this->input_queue.push(task);
			return last_id;
		}

		void start();
		void stop();

		template<typename Tout>
		Tout& get_result()
		{
			auto task = this->output_queue.pop().get();
			return static_cast<TypedTask<Tout>*>(task)->get_output();
		}

	};
}