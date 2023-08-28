#pragma once

#include <vector>
#include <memory>

#include <ff/ff.hpp>
#include <iostream>
#include <chrono>
#include "ff/node.hpp"
#include "ff/farm.hpp"
#include "Task.hpp"
#include "SharedQueue.hpp"
#include "MonitorInfo.hpp"

#include "fastflow/Monitor.hpp"
#include "fastflow/Executor.hpp"


namespace fastflow
{
	class InputSource : public ff::ff_node_t<ITask>
	{
	public:
		explicit InputSource(SharedQueue<ITask*>& input_queue) : input_queue{input_queue}
		{}

		ITask* svc(ITask*) override
		{
			while(true)
			{
				ITask *task = input_queue.pop();
				if(task->is_eos())
				{
					delete task;
					return EOS;
				}
				ff_send_out(task);
			}
		}

//		void svc_end() override
//		{
//			std::cout << "source finished" << std::endl;
//		}

	private:
		SharedQueue<ITask*>& input_queue;
	};

	class Worker : public ff::ff_monode
	{
		void *svc(void *t) override
		{
			Task* task = static_cast<Task* >(t);
			if(task->is_eos())
			{
				// std::cout << "recived artificial EOS" << std::endl;
				return GO_ON;
			}

			// std::cout << "Worker " << this->get_my_id() << " running task " << task->get_id() << std::endl;
			MonitorInfo* info;

			info = new MonitorInfo{InfoType::TASK_STARTED, std::this_thread::get_id(), task->get_id(), high_resolution_clock::now()};
			ff_send_out_to(info, 0);

			task->run();

			info = new MonitorInfo{InfoType::TASK_FINISHED, std::this_thread::get_id(), task->get_id(), high_resolution_clock::now()};
			ff_send_out_to(info, 0);

			ff_send_out_to(task, 1);
			return GO_ON;
		}

//		void svc_end() override
//		{
//			std::cout << "worker " << this->get_my_id() << " finished" << std::endl;
//		}
	};

	class OutputSink : public ff::ff_minode
	{
	public:
		explicit OutputSink(SharedQueue<ITask*>& output_queue) : output_queue{output_queue}
		{}

		void* svc(void* t) override
		{
			auto* task = static_cast<ITask*>(t);
			if(task->is_eos())
			{
				throw std::runtime_error("EOS received by collector");
			}
			output_queue.push(task);

			auto* info = new MonitorInfo{InfoType::TASK_PUSHED, std::this_thread::get_id(), task->get_id(), high_resolution_clock::now()};

			return info;
		}

//		void svc_end() override
//		{
//			std::cout << "sink finished" << std::endl;
//		}

	private:
		SharedQueue<ITask*>& output_queue;
	};


	class MonitoredFarm : public IMonitoredFarm
	{
	private:
		ff::ff_farm farm;
		std::shared_ptr<unsigned int> n_workers;
		ff::ff_pipeline pipeline;
		unsigned long int last_id = 0;
		SharedQueue<ITask*> input_queue;
		SharedQueue<ITask*> output_queue;

	public:
		explicit MonitoredFarm(unsigned short int n_workers, unsigned short int max_workers, std::shared_ptr<Monitor> monitor, std::shared_ptr<Executor> executor) :
			farm{false, 10 * max_workers, 10 * max_workers, false, max_workers, true},
            pipeline{}, input_queue{}, output_queue{}
		{
			this->n_workers = std::make_shared<unsigned int>(n_workers);
			std::vector<ff::ff_node*> workers;
			for (unsigned int i = 0; i < max_workers; ++i)
			{
				workers.push_back(new Worker());
			}

			auto* collector = new OutputSink(this->output_queue);

			this->farm.add_emitter(executor.get());
			this->farm.add_workers(workers);
			// this->farm.add_collector(collector);
			this->farm.remove_collector();

			// this->farm.wrap_around();
			// this->farm.cleanup_all();
			this->farm.set_scheduling_ondemand();

			this->farm.getlb()->addManagerChannel(executor->getChannel());

			auto* pipe_1 = new ff::ff_pipeline();
			auto* pipe_2 = new ff::ff_pipeline();

			monitor->set_n_worker_ptr(this->n_workers);
			executor->set_n_worker_ptr(this->n_workers);
			pipe_1->add_stage(monitor.get());
			pipe_1->add_stage(this->farm);
			pipe_1->wrap_around();

			pipe_2->add_stage(pipe_1);
			pipe_2->add_stage(collector);
			pipe_2->wrap_around();

			this->pipeline.add_stage(new InputSource(this->input_queue));
			this->pipeline.add_stage(pipe_2);
			//this->pipeline.cleanup_nodes();
		}

		unsigned long int add_task(std::function<void()> func, void* output) override
		{
			this->last_id++;
			ITask* task = new Task(last_id, func, output);
			this->input_queue.push(task);

			return last_id;
		}

		void start() override
		{
			this->pipeline.run_then_freeze();
		}

		void stop() override
		{
			ITask* eos = new EoSTask();
			this->input_queue.push(eos);
			this->pipeline.wait_freezing();
		}


		void* get_result() override
		{
			auto task = this->output_queue.pop();
			void* result = dynamic_cast<Task*>(task)->get_output();
			delete task;
			return result;
		}
	};
}