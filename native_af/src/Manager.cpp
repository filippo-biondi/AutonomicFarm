#include "Manager.hpp"


namespace native_af
{
	Manager::Manager(MonitoredFarm &farm, Monitor& monitor, Analyser analyser, Planner planner, Executor executor, double frequency, double time_span) :
	farm{farm}, monitor{monitor}, analyser{analyser}, planner{planner}, executor{executor},
	time_span{time_span}, wait_time{std::chrono::duration<double>{1.0 / frequency}}
	{}

	/**
	 * First start the Monitor and then the thread executing the MAPE loop.
	 */
	void Manager::start()
	{
		this->monitor.start();
		this->manager_thread = std::thread{&Manager::manager_func, this};
	}

	/**
	 * Stop the thread executing the MAPE loop and then stop the Monitor.
	 */
	void Manager::stop()
	{
		this->stop_thread = true;
		this->monitor.stop();
		this->manager_thread.join();
	}

	/**
	 * Run in loop the following action:
	 * - log the metrics of the Monitor
	 * - run the Analyzer
	 * - run the Planner with the result of the Analyzer
	 * - run the Executor with the result of the Planner
	 * - wait for the time specified by the manager frequency
	 */
	void Manager::manager_func()
	{
		while (true)
		{
			this->monitor.log(this->time_span);
			auto [action, throughput, arrival_frequency, task_time, queue_size] = this->analyser.analyse();
			auto worker_diff = planner.plan(action, throughput, arrival_frequency, task_time, queue_size);
			if(worker_diff > 0)
			{
				this->executor.add_workers(worker_diff);
			}
			else if(worker_diff < 0)
			{
				this->executor.remove_workers(-worker_diff);
			}

			std::this_thread::sleep_for(this->wait_time);

			if(this->stop_thread) { break; }
		}
	}
}