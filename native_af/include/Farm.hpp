#pragma once

#include <memory>
#include <queue>
#include <thread>

#include "TypedTask.hpp"
#include "Emitter.hpp"
#include "Collector.hpp"


namespace native_af
{
	class Farm
	{
	private:
		unsigned int n_workers;
		std::vector<std::thread> workers;
		SharedQueue<std::shared_ptr<Task>> input_queue;
		SharedQueue<std::shared_ptr<Task>> output_queue;
//		Emitter emitter;
//		Collector collector;

		void worker_func();

	public:

		explicit Farm(unsigned int n_workers);

		template<typename Tin, typename Tout>
		void add_task(std::function<Tout(Tin)> func, Tin& input)
		{
			auto task = std::shared_ptr<Task>(new TypedTask<Tout>(func, input));
			this->input_queue.push(task);
		}

		void add_task(std::shared_ptr<Task> task);

		void stop();

		template<typename Tout>
		Tout& get_result()
		{
			auto task = this->output_queue.pop().get();
			return static_cast<TypedTask<Tout>*>(task)->get_output();
		}


	};
}