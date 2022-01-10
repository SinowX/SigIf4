#ifndef LOGGER_H
#define LOGGER_H

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <memory>

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

// T should be One Byte Type
template<typename T>
std::stringstream& ToHexBytes(const T* ptr, const ssize_t len)
{
	
	LOGDBG<<"ToHexBytes Start";
	LOGDBG<<"Len: "<<len;
	
	static std::stringstream ss;
	ss.str("");
	ss.clear();
	
	if(sizeof(T)==1)
	{
		char achar[2];
		for(int i=0;i<len;i++)
		{
			/* LOGDBG<<"In Loop"; */
			sprintf(achar,"%02x", ptr[i]);
			ss<<achar;
		}
	}
	LOGDBG<<"ToHexBytes End";
	return ss;
}


#endif
