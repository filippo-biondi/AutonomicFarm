#include "MonitorLogger.hpp"

#include <utility>
#include <iomanip>



MonitorLogger::MonitorLogger() : streams{}
{}

MonitorLogger::MonitorLogger(std::vector<std::ostream*> streams) : streams{std::move(streams)}
{}

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
		for(auto &elem: values)
		{
			if(elem.second > 1e4 || elem.second < -1e4)
			{
				*stream << elem.first << ": " << std::setw(6) << std::setprecision(0) << std::scientific << elem.second << '\t';
			}
			else
			{
				*stream << elem.first << ": " << std::setw(6) << std::setprecision(2) << std::fixed << elem.second
				        << '\t';
			}
		}
		*stream << '\n';
	}
}
