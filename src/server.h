#ifndef SERVER_H
#define SERVER_H

#include <httpserver.hpp>
#include "backend.h"
#include "conf_mgr.h"
#include "logger.h"

using namespace httpserver;

const std::string get_Result(const std::string&);


const std::shared_ptr<http_response> not_found_route(const http_request& req)
{
	LOGINFO<<"Not Allowed Method: "<<req.get_method()<<" Body: "<<req.get_content();
	return std::shared_ptr<string_response>(new string_response("Not Found\n",404,"text/plain"));
}

const std::shared_ptr<http_response> not_allowed_method(const http_request& req)
{
	LOGINFO<<"Not Allowed Method: "<<req.get_method()<<" Body: "<<req.get_content();
	return std::shared_ptr<string_response>(new string_response("Not Allowed Method\n",405,"text/plain"));
}

class SigInterfaceResource:public http_resource
{
	public:
		static SigInterfaceResource& get_instance()
		{
			static SigInterfaceResource instance_;
			return instance_;
		}
		SigInterfaceResource(SigInterfaceResource&)=delete;
		void operator=(SigInterfaceResource&)=delete;
		~SigInterfaceResource()=default;
		const std::shared_ptr<http_response> render_POST(const http_request& req)
		{
			LOGINFO<<"Get One Request";
				return std::shared_ptr<string_response>(
						new string_response(ProcessRequest(req.get_content()), 200 ,"text/json"));
		}
	private:
		SigInterfaceResource()=default;
};


void server()
{
	uint16_t listen_port = ConfMgr::getInstance().get_listen_port();

	webserver ws = create_webserver(listen_port)
		.max_connections(100)
		.connection_timeout(180)
		.not_found_resource(not_found_route)
		.method_not_allowed_resource(not_allowed_method)
		.start_method(http::http_utils::THREAD_PER_CONNECTION);
	ws.register_resource("/sig_interface", &SigInterfaceResource::get_instance());
	LOGINFO<<"Server Started. "<<"Listenning at port:"<<listen_port;
	ws.start(true);
}
#endif
