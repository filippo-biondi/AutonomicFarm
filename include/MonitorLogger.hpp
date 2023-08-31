#pragma once

#include <iostream>
#include <vector>
#include <memory>


/**
 * Class that implements logging of Monitor metrics on custom output streams.
 */
class MonitorLogger
{
protected:
	std::vector<std::ostream*> streams;
	bool first_log = true;
	bool log_cout;

public:
	/**
	 * MonitorLogger constructor for empty logger.
	 */
	MonitorLogger();

	/**
	 * Basic MonitorLogger constructor.
	 * @param streams the output streams on which the MonitorLogger will log.
	 * @param log_cout whether or not log metrics also on standard output
	 */
	explicit MonitorLogger(std::vector<std::ostream*> streams, bool log_cout);

	/**
	 * Log the string-value pairs on the streams specified in the constructor.
	 * @param value the string-value paris to be logged
	 */
	virtual void log(const std::vector<std::pair<std::string,double>> &values);
};