#ifndef RECEIVER_H
#define RECEIVER_H

#include "connection.h"
#include "sys/epoll.h"
#include <iostream>
#include "conf_mgr.h"
#include "logger.h"

std::string Fd2Ipv4(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof addr;
	if(getpeername(fd, (struct sockaddr*)&addr,&addr_len)<0)
	{
		LOGERROR<<"Invalid Socket Fd to Get Ipv4";
		return "";
	}
	LOGINFO<<"Transform Success";
	return std::string(inet_ntoa(addr.sin_addr));
}

void receiver()
{
	LOGINFO<<"Called receiver()";
	int epfd = epoll_create(1024);
	struct epoll_event ev_tpl;
	bzero(&ev_tpl, sizeof(ev_tpl));
	ev_tpl.events=EPOLLIN|EPOLLET|EPOLLERR|EPOLLRDHUP;

	struct epoll_event active_ev;
	int active_number = 0;

	ConnInfoPtr conn = std::make_shared<ConnInfo>(ConfMgr::getInstance().get_upper_addr(),
			ConfMgr::getInstance().get_upper_port());
	ConnMap::getInstance().Add(conn);
	LOGINFO<<"ConnMap Add fd: "<<conn->GetFd()<<" address: "<<conn->GetIpv4()<<":"<<conn->GetPort();

	epoll_ctl(epfd, EPOLL_CTL_ADD, conn->GetFd(), &ev_tpl);
	LOGINFO<<"Epoll Add fd: "<<conn->GetFd()<<" address: "<<conn->GetIpv4()<<":"<<conn->GetPort();

	LOGDBG<<"reach here";

	while(true)
	{
		LOGINFO<<"Receiver Looping";

		active_number = epoll_wait(epfd, &active_ev, 1024, -1);
		
		LOGDBG<<"Active Number: "<<active_number;

		if(active_number>0)
		{
			int fd = active_ev.data.fd;
			LOGINFO<<"Get Events";
			if(fd<0) 
				LOGWARN<<"Invalid fd: "<<fd;
			LOGINFO<<"Fd: "<<fd;
			uint32_t events = active_ev.events;
			if(events & EPOLLOUT){
				LOGINFO<<"EPOLLOUT Event";
				auto connection = ConnMap::getInstance().Get(Fd2Ipv4(fd));
				if(connection==nullptr)
				{
					LOGWARN<<"connection is nullptr";
					continue;
				}
				connection->ReceiveAll();
				LOGINFO<<"Conn "<<connection->GetIpv4()<<":"
					<<connection->GetPort()<<"Received All Available Packets";
				while(!connection->GetPackQ().empty()){

					auto pack = connection->GetPackQ().front();
					std::shared_ptr<InsParser> parser =
						std::make_shared<InsParser>();
					parser->Parse(
							reinterpret_cast<const unsigned char*>(pack.c_str()),
						 	pack.size());

					TaskMap::ItBool task;

					while(!(task = TaskMap::getInstance()
								.Get(parser->GetInsName())).valid)
					{
						// 10s to be expired
						if(time(nullptr)-task.it->second->timestamp<5){
							task.it->second->mtx->lock();
							task.it->second->parser=parser; // send parser to server
							task.it->second->mtx->unlock();
							task.it->second->condi->notify_one();
						}
						TaskMap::getInstance().Drop(task.it);
					}
				}
				continue;
			}
			if(events & EPOLLERR){
				LOGWARN<<"EPOLLERR Event";
				auto ipv4 = Fd2Ipv4(fd);
				if(ipv4.empty())
				{
					LOGWARN<<"Transform fd to ipv4 failed";
				}
				else
				{
					LOGDBG<<"EPOLLERR fd: "<<fd<<"; ipv4: "<<ipv4;
					ConnMap::getInstance().Drop(Fd2Ipv4(fd));
					LOGINFO<<"ConnMap Droped "<<ipv4;
				}
				continue;
			}

			LOGWARN<<"Unknown Events";
			
		}
	}
	/* while(true) */
	/* { */
	/* 	active_number = epoll_wait(epfd, active_ev, 1024, -1); */
	/* 	for(int i=0; i<active_number; i++) */
	/* 	{ */
	/* 		int fd = active_ev[i].data.fd; */
	/* 		uint32_t events = active_ev[i].events; */

	/* 		if(events & EPOLLIN){ */
	/* 			auto conn = ConnMap::getInstance().Get(Fd2Ipv4(fd)); */		
	/* 			conn->ReceiveAll(); */
	/* 			while(!conn->GetPackQ().empty()) */
	/* 			{ */
	/* 				auto pack = conn->GetPackQ().front(); */
	/* 				std::shared_ptr<InsParser> parser = */
	/* 					std::make_shared<InsParser>(); */
	/* 				parser->Parse( */
	/* 						reinterpret_cast<const unsigned char*>(pack.c_str()), */
	/* 					 	pack.size()); */

	/* 				TaskMap::ItBool task; */
	
	/* 				while(!(task = TaskMap::getInstance() */
	/* 							.Get(parser->GetInsName())).valid) */
	/* 				{ */
	/* 					// 10s to be expired */
	/* 					if(time(nullptr)-task.it->second->timestamp<10){ */
	/* 						task.it->second->mtx->lock(); */
	/* 						task.it->second->parser=parser; */
	/* 						task.it->second->mtx->unlock(); */
	/* 						task.it->second->condi->notify_one(); */
	/* 					} */
	/* 					TaskMap::getInstance().Drop(task.it); */
	/* 				} */
	/* 			} */
	/* 		} */

	/* 		if(events & EPOLLERR){ */
	/* 			ConnMap::getInstance().Drop(Fd2Ipv4(fd)); */
	/* 		} */

	/* 	} */
	/* } */
}

#endif
