#include "ENetClient.h"
#include <iostream>
#include <Printer.h>



ENetClient::~ENetClient()
{

	enetpp::global_state::get().deinitialize();
}

void ENetClient::initialise()
{
	enetpp::global_state::get().initialize();
    client.connect(enetpp::client_connect_params()
    .set_channel_count(1)
    .set_server_host_name_and_port("localhost", 8888));

    Printer() << "Who are you?: ";
   // std::getline(std::cin, username);
}

void ENetClient::run()
{
	std::thread th(&ENetClient::input, this);
	th.detach();

    while (client.is_connecting_or_connected())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		using namespace std::placeholders;
		client.consume_events(
		        std::bind(&ENetClient::connection, this),
		        std::bind(&ENetClient::disconnection, this),
		        std::bind(&ENetClient::data, this, _1, _2));

        while(msg_queue.size())
        {
            std::lock_guard<std::mutex> lock(msg_queue_mtx);
            const auto& txt = msg_queue.front();
            assert(sizeof(char) == sizeof(enet_uint8));
            client.send_packet(0, reinterpret_cast<const enet_uint8*>(txt.data()),
                               txt.length(), ENET_PACKET_FLAG_RELIABLE);
            msg_queue.pop();
        }
	}

	exiting = true;
}

void ENetClient::input()
{
  while (!exiting)
  {
        std::string txt;
        std::getline(std::cin, txt);

        std::lock_guard<std::mutex> lock(msg_queue_mtx);

        msg_queue.push(std::move(txt));



  }
}

void ENetClient::connection()
{

	Printer() << "Connection Successful" << std::endl;
	//std::to_string(client.)

}

void ENetClient::disconnection()
{

    Printer() << "Disconnected from Server" << std::endl;
}

void ENetClient::data(const enet_uint8 *data, size_t data_size)
{

    std::string msg(reinterpret_cast<const char*>(data), data_size);
    Printer() << msg;
    Printer() << std::endl;

}
