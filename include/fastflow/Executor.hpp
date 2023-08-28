#pragma once

#include <ff/ff.hpp>

#include "Task.hpp"
#include "IExecutor.hpp"


namespace fastflow
{
	class Executor : public ff::ff_monode, public IExecutor
	{
	private:
		std::shared_ptr<unsigned int> n_workers_ptr;
		unsigned int max_workers;
		std::vector<bool> sleeping;
		std::vector<bool> exited;
		ff::ff_buffernode cmd_channel;

		unsigned int select_sleeping_worker();
		unsigned int select_awake_worker();


	public:
		/**
		 * Basic Executor constructor.
		 * @param farm the farm on which the executor operate
		 */
		explicit Executor(unsigned int current_workers);

		int svc_init() override;

		void* svc(void* t) override;

//		void svc_end() override
//		{
//			std::cout << "Executor exited" << std::endl;
//		}

		void eosnotify(ssize_t id) override;

		/**
		 * Add n workers to the farm.
		 * @param n number of workers to add
		 */
		void add_workers(unsigned int n) override;

		/**
		 * Remove n workers from the farm.
		 * @param n number of workers to remove
		 */
		void remove_workers(unsigned int n) override;

		ff::ff_buffernode* getChannel()
		{
			return &this->cmd_channel;
		}

		void set_n_worker_ptr(std::shared_ptr<unsigned int>& ptr);
	};
}