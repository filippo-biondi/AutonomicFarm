#pragma once

#include <iostream>
#include <vector>
#include <memory>


namespace native_af
{
	class MonitorLogger
	{
	protected:
		std::vector<std::ostream*> streams;

	public:
		MonitorLogger();
		explicit MonitorLogger(std::vector<std::ostream*> streams);

		virtual void log(const std::vector<std::pair<std::string,double>> &values);
	};
}