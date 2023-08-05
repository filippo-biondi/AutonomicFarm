#include "MonitorLogger.hpp"

#include <utility>
#include <iomanip>


namespace native_af
{
	MonitorLogger::MonitorLogger() : streams{}
	{}

	MonitorLogger::MonitorLogger(std::vector<std::ostream*> streams) : streams{std::move(streams)}
	{}

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
}