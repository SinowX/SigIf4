#include "conf_mgr.h"
#include <thread>
#include "server.h"
#include "receiver.h"


int main()
{
	ConfMgr::getInstance().ReadConfig("config.json");
	if(Logger::Init(ConfMgr::getInstance().get_log_path())==0)
		LOGDBG<<"Logger Init Success";
	else{
		LOGFATAL<<"Logger Init Failed";
		abort();
	}

	MachineInfo *info = ConfMgr::getInstance().get_machine().XINGZHI.head;
  while(info)
  {
    ConnInfoPtr conn = std::make_shared<ConnInfo>(info->ipv4, info->port);
    ConnMap::getInstance().Add(conn);
		LOGINFO<<"Added "<<info->ipv4<<":"<<info->port<<" into ConnMap";
    info=info->next;
  }

	std::thread svr_thrd(server);
	std::thread rcvr_thrd(receiver);

	svr_thrd.join();
	rcvr_thrd.join();

	return 0;
	LOGFATAL<<"Should not reach Here!!";
}
