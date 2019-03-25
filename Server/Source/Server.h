#pragma once
#include <enetpp/server.h>

class Server
{
private:
    struct server_client
    {
        unsigned int _uid;
        unsigned int get_id() const {return _uid; }
    };

public:
	Server() = default;
	~Server();

	void initialise();
	void run();

private:
    enetpp::server<server_client> network_server;
    unsigned int next_uid = 0;
};


