#pragma once

#include <iomanip>
#include "Monitor.hpp"
#include "Action.hpp"


namespace native_af
{
	class Analyser
	{
	private:
		Monitor& monitor;
		double epsilon = 0.5;
		double estimated_overhead = 0.0;
		unsigned int queue_upper_limit = 100;
		unsigned int queue_lower_limit = 10;
		double time_span;
		Action last_action = Action::NO_ACTION;

	public:
		Analyser(Monitor& monitor, double epsilon, unsigned int queue_upper_limit, unsigned int queue_lower_limit, double average_time_span);

		std::tuple<Action, double, double, double, unsigned int> analyse();
	};
}