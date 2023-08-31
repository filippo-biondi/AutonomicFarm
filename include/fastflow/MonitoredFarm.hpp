#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <chrono>

#include <ff/ff.hpp>
#include "ff/node.hpp"
#include "ff/farm.hpp"

#include "Task.hpp"
#include "SharedQueue.hpp"
#include "MonitorInfo.hpp"
#include "IMonitoredFarm.hpp"
#include "fastflow/Monitor.hpp"
#include "fastflow/Executor.hpp"


namespace fastflow
{
	class InputSource : public ff::ff_node_t<ITask>
	{
	public:
		explicit InputSource(SharedQueue<ITask*>& input_queue);

		ITask* svc(ITask*) override;

	private:
		SharedQueue<ITask*>& input_queue;
	};


	class Worker : public ff::ff_monode
	{
		void *svc(void *t) override;

		void eosnotify(ssize_t) override;
	};


	class OutputSink : public ff::ff_minode
	{
	public:
		explicit OutputSink(SharedQueue<ITask*>& output_queue);

		void* svc(void* t) override;

	private:
		SharedQueue<ITask*>& output_queue;
	};

	class Monitor;
	class Executor;

	class MonitoredFarm : public IMonitoredFarm
	{
	private:
		std::atomic<unsigned int> n_workers;
		ff::ff_farm farm;
		ff::ff_pipeline pipe_1;
		ff::ff_pipeline pipe_2;
		ff::ff_pipeline pipeline;
		unsigned long int last_id = 0;
		SharedQueue<ITask*> input_queue;
		SharedQueue<ITask*> output_queue;
		std::unique_ptr<InputSource> input_source;
		std::unique_ptr<OutputSink> output_sink;

	public:
		friend class Executor;
		friend class Monitor;

		explicit MonitoredFarm(unsigned short int n_workers, unsigned short int max_workers, std::shared_ptr<Monitor> monitor, std::shared_ptr<Executor> executor);

		~MonitoredFarm() override;

		unsigned long int add_task(ITask* task) override;

		void start() override;

		void stop() override;

		std::shared_ptr<ITask> get_result() override;
	};
}