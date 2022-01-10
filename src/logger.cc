#include "logger.h"

Logger::Logger(const std::string initFilePath)
{
	log4cpp::PropertyConfigurator::configure("log4cpp.properties");
	root = &log4cpp::Category::getRoot();
}

Logger* Logger::instance_ = 0;

	
