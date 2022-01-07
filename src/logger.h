#ifndef LOGGER_H
#define LOGGER_H

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

class Logger
{
	public:
		static int Init(const std::string initFilePath)
		{
			if(!instance_)
			{
				instance_ = new Logger(initFilePath);
				return 0;
			}else{
				return -1;
			}
		}	
		
		static log4cpp::Category& Get()
		{
			return *instance_->root;
		}

		void operator=(const Logger&) = delete;
		Logger(const Logger&) = delete;
		~Logger()=default;

	private:
		static Logger *instance_;
		Logger(const std::string initFilePath);
		log4cpp::Category* root;
};

#define LOGDBG Logger::Get()<<log4cpp::Priority::DEBUG<<__func__<<"():"<<__LINE__<<" "
#define LOGINFO Logger::Get()<<log4cpp::Priority::INFO<<__func__<<"():"<<__LINE__<<" "
#define LOGWARN Logger::Get()<<log4cpp::Priority::WARN<<__func__<<"():"<<__LINE__<<" "
#define LOGERROR Logger::Get()<<log4cpp::Priority::ERROR<<__func__<<"():"<<__LINE__<<" "
#define LOGFATAL Logger::Get()<<log4cpp::Priority::FATAL<<__func__<<"():"<<__LINE__<<" "

#endif
