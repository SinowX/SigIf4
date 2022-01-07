#include "conf_mgr.h"
/* #include "INIReader.h" */
/* #include "log_mgr.h" */

/* ConfMgr::ConfMgr() */
/* { */
/* 	bzero(&machine_config_, sizeof(machine_config_)); */
/* } */


static const char* ReadFile(const char* path)
{
	
	int fd = -1;
	if((fd = open(path, O_RDONLY))<0)
	{
		/* LOGERROR("open config file failed"); */
		abort();
	}

	int file_len = lseek(fd, 0, SEEK_END);
	if((file_len = lseek(fd, 0, SEEK_END))<0)
	{
		/* LOGERROR("cant get file size"); */
		abort();
	}

	lseek(fd,0, SEEK_SET);
	char* content = new char[file_len+1];

	int n = read(fd, content, file_len);

	if(n!=file_len)
	{
		/* LOGERROR("read file failed"); */
	}

	content[file_len]='\0';
	
	return content;
}

static int ValidateConfig(const nlohmann::basic_json<>& j)
{
	if(j["basic"].is_null()
			||j["basic"]["listen_ipv4"].is_null()
			||j["basic"]["listen_port"].is_null()
			||j["basic"]["upper_ipv4"].is_null()
			||j["basic"]["upper_port"].is_null()
			||j["basic"]["log_path"].is_null())
		return false;

	if(!j["basic"]["listen_ipv4"].is_string()
			||!j["basic"]["listen_port"].is_string()
			||j["basic"]["upper_ipv4"].is_string()
			||j["basic"]["upper_port"].is_string()
			||!j["basic"]["log_path"].is_string())
		return false;

	/* if(j["io_receiver"].is_null() */
	/* 		||j["io_receiver"]["amount"].is_null()) */
	/* 	return false; */
	/* if(!j["io_receiver"]["amount"].is_number_integer()) */
	/* 	return false; */

	/* if(j["worker"].is_null() */
	/* 		||j["worker"]["amount"].is_null()) */
	/* 	return false; */
	/* if(!j["worker"]["amount"].is_number_integer()) */
	/* 	return false; */

	if(j["machine"].is_null()
			||!j["machine"].is_array())
		return false;

	for(int i=0; i<j["machine"].size();i++)
	{
		if(j["machine"][i]["type"].is_null()
				||j["machine"][i]["ipv4_addr"].is_null()
				||j["machine"][i]["port"].is_null())
			return false;
		if(!j["machine"][i]["type"].is_string()
				||!j["machine"][i]["ipv4_addr"].is_string()
				||!j["machine"][i]["port"].is_number_unsigned())
			return false;
	}

	return true;
}

//
int ConfMgr::ReadConfig(const char *path)
{
	auto j = json::parse(ReadFile(path));
	if(ValidateConfig(j)<0)
	{
		/* LOGERROR("parse config failed"); */
		abort();
	}

	auto basic = j["basic"];
	
	listen_addr_ = basic["listen_ipv4"].get<std::string>();
	port_ = basic["listen_port"].get<uint16_t>();
	log_path_ = basic["log_path"].get<std::string>();
	
	upper_addr_ = basic["upper_ipv4"].get<std::string>();
	upper_port_ = basic["upper_port"].get<uint16_t>();
	

	/* io_rcvr_amount_ = j["io_receiver"]["amount"].get<int>(); */
	/* work_amount_ = j["worker"]["amount"].get<int>(); */

	auto machine_arr = j["machine"];
	
	for(int i=0; i<machine_arr.size(); i++)
	{
		auto machine = machine_arr[i];
		/* auto machine_type = machine_arr[i]["type"].get<std::string>(); */
		auto machine_type = machine["type"].get<std::string>();
		auto Add_Device = [&](EachMachineType* machine_struct,
				nlohmann::basic_json<>& machine ){
			

			MachineInfo* info=nullptr;
			info = new MachineInfo;
			info->id = 0;// not use
			strncpy(info->ipv4, machine["ipv4_addr"]
					.get<std::string>().c_str(), 16);
			info->port = machine["port"].get<uint16_t>();
			info->next = nullptr;
			if(machine_struct->head==nullptr)
			{
				machine_struct->head=info;
			}else{
				MachineInfo* loc = machine_struct->head;
				while(loc->next!=nullptr) loc=loc->next;
				loc->next=info;
			}	
		};

		if(!machine_type.compare("XINGZHI"))
		{
			/* MachineInfo* head=machine_config_.XINGZHI.head; */
			Add_Device(&machine_config_.XINGZHI, machine);
			machine_config_.XINGZHI.number++;
		/* }else */
		/* 	if(!machine_type.compare("HAIXIN")) */
		/* { */
		/* 	MachineInfo* info=machine_config_.HAIXIN.head; */
		/* 	Add_Device(info, machine); */
		/* 	machine_config_.HAIXIN.number++; */
		}else
		{
			/* LOGWARN("Unknown machine Type"); */
		}
	}
	read_success_=true;
	return 0;
}
