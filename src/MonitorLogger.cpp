#include "MonitorLogger.hpp"

#include <utility>
#include <iomanip>



MonitorLogger::MonitorLogger() : streams{}, log_cout{false}
{}

MonitorLogger::MonitorLogger(std::vector<std::ostream*> streams, bool log_cout) : streams{std::move(streams)}, log_cout{log_cout}
{
	this->streams.push_back(&std::cout);
}

/**
 * Log all the string-value pair with the string followed by ": " followed by the value and separating each pair witha a tab.
 */
void MonitorLogger::log(const std::vector<std::pair<std::string, double>> &values)
{
	for(auto &stream: this->streams)
	{
		if(!stream)
		{
			continue;
		}
		if(this->first_log)
		{
			for(auto &elem: values)
			{
				*stream<< std::setw(elem.first.size()) << elem.first << '\t';
			}
			*stream << '\n';
		}
		for(auto &elem: values)
		{
			if(elem.second > 1e4 || elem.second < -1e4)
			{
				*stream << std::setw(elem.first.size()) << std::setprecision(2) << std::scientific << elem.second << '\t';
			}
			else
			{
				*stream << std::setw(elem.first.size()) << std::setprecision(3) << std::fixed << elem.second << '\t';
			}
		}
		if(stream == streams.back() && this->log_cout)
		{
			*stream << '\r' << std::flush;
		}
		else
		{
			*stream << '\n';
		}
	}

	if(this->first_log)
	{
		this->first_log = false;
	}
}
