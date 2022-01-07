// Configuration Manager
// 读取配置文件，并提供全局访问配置信息功能
#ifndef CONF_MGR_H
#define CONF_MGR_H

#include <cstdint>
/* #include "type_safe.h" */
#include "variant_const.h"
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/* #include "INIReader.h" */
/* #include "log_mgr.h" */
#include <nlohmann/json.hpp>
#include "logger.h"


using json=nlohmann::json;
typedef struct MachineInfo
{
	char ipv4[16];
	uint16_t port;
	int id;
	struct MachineInfo *next;
}MachineInfo;

typedef struct EachMachineType
{
	int number;
	MachineInfo* head;
}EachMachineType;

typedef struct
{
	EachMachineType XINGZHI;
	EachMachineType HAIXIN;
	EachMachineType HAIKANG;
	EachMachineType XIMENZI;
	EachMachineType DEVICE5;
}Machine;

class ConfMgr
{
	public:
		static ConfMgr& getInstance()
		{
			static ConfMgr instance_;
			return instance_;
		}
		ConfMgr(ConfMgr&)=delete;
		void operator=(ConfMgr&)=delete;
		~ConfMgr()
		{
		}

		// read config. return 0 for success, -1 for error
		int ReadConfig(const char* path);

		// if has not read config ,return 0 or nullptr
		/* inline int get_worker_amount() const */
		/* 	{if(read_success_) return work_amount_; else return 0;} */
		/* inline int get_io_rcvr_amount() const */ 
		/* 	{if(read_success_) return io_rcvr_amount_; else return 0;} */
		inline const Machine& get_machine() const
		  	{return machine_config_;}
		inline uint16_t get_listen_port() const
			{if(read_success_) return port_; else return 0;}
		inline const std::string get_listen_addr() const
			{if(read_success_) return listen_addr_; else return nullptr;}
		inline uint16_t get_upper_port() const
			{if(read_success_) return upper_port_; else return 0;}
		inline const std::string get_upper_addr() const
			{if(read_success_) return upper_addr_; else return nullptr;}

		inline const std::string get_log_path() const
			{
				if(read_success_)
			 		return log_path_;
				else return nullptr;}


	private:
		ConfMgr()
		{
			memset(&machine_config_, 0, sizeof(machine_config_));
		}
		bool read_success_{false};
		/* int work_amount_{0}; */
		/* int io_rcvr_amount_{0}; */
		Machine machine_config_;
		/* char* listen_addr_{nullptr}; */
		std::string listen_addr_;
		uint16_t port_{0};
		std::string upper_addr_;
		uint16_t upper_port_{0};
		std::string log_path_;
};

#endif
