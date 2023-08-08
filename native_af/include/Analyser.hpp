#pragma once

#include <iomanip>
#include "Monitor.hpp"
#include "Action.hpp"


namespace native_af
{
	/**
	 * Class that implements the Analyzer component of the MAPE loop.
	 */
	class Analyser
	{
	private:
		Monitor& monitor;
		double epsilon = 0.5;
		double estimated_overhead = 0.0;
		unsigned int queue_upper_limit = 100;
		unsigned int queue_lower_limit = 10;
		std::chrono::duration<double> time_span;
		Action last_action = Action::NO_ACTION;

	public:
		/**
		 * Basic Analyzer constructor.
		 * @param monitor Monitor from which metrics to base the decisions on are extracted
		 * @param epsilon Estimated overhead of the farm caused by synchronization of worker when accessing shared queues
		 * @param queue_upper_limit If the queue size is greater than this value, the Analyzer will output Action::RECOVER_QUEUE until queue size become smaller that queue_lower_limit
		 * @param queue_lower_limit If the queue size is lower than this value, the Analyzer will not output Action::RECOVER_QUEUE anymore.
		 * @param average_time_span Time span of the moving average requested from the monitor by the Analyzer
		 */
		Analyser(Monitor& monitor, double epsilon, unsigned int queue_upper_limit, unsigned int queue_lower_limit, double average_time_span);

		/**
		 * Analyse the metrics extracted from the monitor and return the action to be taken by the Planner.
		 * @return a tuple containing the action to be taken, the throughput achievable with the current number of worker, the arrival frequency, the average time required to complete a task and the input queue size
		 */
		std::tuple<Action, double, double, double, unsigned int> analyse();
	};
}