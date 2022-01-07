#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <cstdint>
#include <cstring>
#include <string>
/* #include <vector> */
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>
#include <memory>
#include "instruction_manager.h"
/* using std::string; */
#include "logger.h"
#include <ostream>
#include <sstream>

class noncopyable
{
	public:
	noncopyable(const noncopyable&)=delete;
	void operator=(const noncopyable&)=delete;

	protected:
	noncopyable() = default;
	~noncopyable() = default;

};



class PackQ: noncopyable
{
	public:
		std::string Get()
		{
			std::lock_guard<std::mutex> lk(mtx_);
			std::string res = pack_q_.front();
			pack_q_.pop();
			return res;
		}
		// return pack_q size
		int Push(std::string& pack)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			pack_q_.push(pack);
			return pack_q_.size();
		}

		int Size() const {return pack_q_.size();}

	private:
		std::queue<std::string> pack_q_;
		std::mutex mtx_;
};

class ConnInfo: noncopyable
{
	public:
		ConnInfo(std::string ipv4_addr, const uint16_t port)
			: port_(port), ipv4_addr_(ipv4_addr)
		{
			fd_ = InitUdpSocket(port, ipv4_addr.c_str());
			addr_ = InitSocketAddr(port, ipv4_addr.c_str());
		}
		
		// wait for the 1st pack for wait seconds
		// return success received pack number
		int ReceiveAll()
		{
			LOGDBG<<"Trying ReceiveALl Packet";
			static char read_buff[1000];
			int n=-1;
			while((n=recvfrom(fd_, read_buff, 1000, 0, NULL, 0)>0))
			{
				LOGDBG<<"Get One Packet "<<n<<"B";
				GetPackQ().push(std::string(read_buff));
			}
			LOGDBG<<"Now Containing "<<GetPackQ().size()<<" Packets";
			return 0;
		}

		int Send(std::string data)
		{
			if(data.size()!=sendto(fd_,data.c_str(), data.size(), 0, (sockaddr*)&addr_, sizeof(addr_)))
			{
				if(data.size()<0)
				{
					/* LOGWARN("send failed"); */
					return -1;
				}else{
					/* LOGWARN("send packet unfinished"); */
					return -1;
				}
			}else{
				return data.size();
			}
		}

		std::queue<std::string>& GetPackQ()
		{
			return recv_pack_q_;
		}


		const std::string& GetIpv4() const {return ipv4_addr_;}
		uint16_t GetPort() const {return port_;}
		int GetFd() const {return fd_;}

	private:
		struct sockaddr_in InitSocketAddr(uint16_t port=0,const char* ipaddr=nullptr)
		{
			struct sockaddr_in addr;
			/* socklen_t addr_len = sizeof(addr); */
			memset(&addr, 0, sizeof(struct sockaddr_in));
			addr.sin_family = AF_INET;
			addr.sin_port=htons(port);
			if(ipaddr!=nullptr)
				addr.sin_addr.s_addr = inet_addr(ipaddr);
			else
				/* addr.sin_addr.s_addr = INADDR_ANY; */
				addr.sin_addr.s_addr = inet_addr("0.0.0.0");
			return addr;
		}

		int InitUdpSocket(uint16_t port=0,const char* ipaddr=nullptr)
		{
			int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if(sockfd<0)
			{
				/* LOGWARN("init socket fd failed"); */
				/* Reset(); */
				return -1;
			}
			struct sockaddr_in addr = InitSocketAddr(port, ipaddr);

			if(bind(sockfd, (struct sockaddr*)&addr,sizeof(addr))==-1)
			{
				/* LOGWARN("bind failed"); */
				/* Reset(); */
				return -1;
			}
			return sockfd;
		}

		std::queue<std::string> recv_pack_q_;
		std::string ipv4_addr_;
		int fd_{0};
		uint16_t port_{0};
		struct sockaddr_in addr_;
};

using ConnInfoPtr = std::shared_ptr<ConnInfo>;

class ConnMap :noncopyable
{
	public:
		static ConnMap& getInstance()
		{
			static ConnMap instance_;
			return instance_;
		}
		ConnInfoPtr Get(std::string key)
		{
			if(key.empty())
			{
				LOGWARN<<"Empty Key";
				return nullptr;
			}else{
				auto it = conn_map_.find(key);
				if(it==conn_map_.end())
				{
					LOGWARN<<"Unable Find With "<<key;
					return nullptr;
				}else{
					return it->second;
				}
			}
		}
		int Add(ConnInfoPtr& ptr)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			conn_map_.insert({ptr->GetIpv4(), ptr});
			return conn_map_.size();
		}

		int Drop(std::string key)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			conn_map_.erase(key);
			return conn_map_.size();
		}
		std::stringstream InspectConnMap()
		{
			std::stringstream ss;
			for(auto it:conn_map_)
			{
				ss<<"key: "<<it.first<<" conn_ip: "<<it.second->GetIpv4()
					<<" conn_port: "<<it.second->GetPort()
					<<" conn_fd: "<<it.second->GetFd()<<"\n";
			}
			return ss;
		}
	private:
		ConnMap()=default;
		// ipv4addr-type (name or id) be the key
		std::map<std::string, ConnInfoPtr> conn_map_;
		std::mutex mtx_;
};


class Task: noncopyable
{
	public:
		Task(std::string ip, std::string type, std::time_t time,
				std::shared_ptr<std::mutex> mt,
				std::shared_ptr<std::condition_variable> cond)
			: ipv4_addr(ip), task_type(type), timestamp(time),
				mtx(mt), condi(cond), parser{nullptr} {}
		std::string ipv4_addr;
		std::string task_type; // see API Wiki
		std::time_t timestamp; // upload timestamp, to check if outdated(5s)
		std::shared_ptr<std::mutex> mtx;
		std::shared_ptr<std::condition_variable> condi;
		std::shared_ptr<InsParser> parser;
};

using TaskPtr = std::shared_ptr<Task>;



class TaskMap: noncopyable
{
	public:
		static TaskMap& getInstance()
		{
			static TaskMap instance_;
			return instance_;
		}
		void Add(TaskPtr ptr)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			task_map_.insert({ptr->ipv4_addr,ptr});
		}
		

	private:
		TaskMap()=default;
		std::multimap<std::string, TaskPtr> task_map_;
		std::mutex mtx_;
	public:
		using ItBool = struct{
			decltype(task_map_)::iterator it;
			bool valid;
		};
		
		ItBool Get(std::string key)
		{
			/* std::lock_guard<std::mutex> lk(mtx_); */
			mtx_.lock();
			auto it = task_map_.find(key);
			mtx_.unlock();

			if(it!=task_map_.end())
				return ItBool{it,true};
			else return ItBool{it,false};
		}
		void Drop(decltype(task_map_)::iterator it)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			task_map_.erase(it);
		}
};

#endif
