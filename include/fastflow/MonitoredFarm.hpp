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

/**
 * Namespace for FastFlow implementation classes.
 */
namespace fastflow
{
	/**
	 * First stage of the pipeline that pop task from the input queue and send them to the Monitor.
	 */
	class InputSource : public ff::ff_node_t<ITask>
	{
	public:
		explicit InputSource(SharedQueue<ITask*>& input_queue);

		ITask* svc(ITask*) override;

	private:
		SharedQueue<ITask*>& input_queue;
	};

	/**
	 * Worker of the farm.
	 */
	class Worker : public ff::ff_monode
	{
		void *svc(void *t) override;

		void eosnotify(ssize_t) override;
	};


	/**
	 * Last stage of the pipeline that push task in the output queue.
	 */
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

	/**
	 * Fastflow implementation of a Farm capable of being monitored and with variable number of workers.
	 */
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

		/**
		 * Basic MonitoredFarm constructor.
		 * @param n_workers initial number of workers
		 * @param max_workers maximum number of workers
		 * @param monitor monitor of the MAPE loop
		 * @param executor executor of the MAPE loop
		 */
		explicit MonitoredFarm(unsigned short int n_workers, unsigned short int max_workers, std::shared_ptr<Monitor> monitor, std::shared_ptr<Executor> executor);

		/**
		 * Basic MonitoredFarm destructor.
		 */
		~MonitoredFarm() override;

		/**
		 * Add a task to be executed by the farm.
		 * @param task pointer to the task to be executed
		 */
		unsigned long int add_task(ITask* task) override;

		/**
		 * Start the execution of the farm.
		 */
		void start() override;

		/**
		 * Stop the execution of the farm.
		 */
		void stop() override;

		/**
		 * Get the first task in the output queue.
		 */
		std::shared_ptr<ITask> get_result() override;
	};
}