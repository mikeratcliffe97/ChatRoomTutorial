#include <Printer.h>
#include "Server.h"

Server::~Server()
{
	network_server.stop_listening();
	enetpp::global_state::get().deinitialize();
}

void Server::initialise()
{
enetpp::global_state::get().initialize();

	auto init_client_func =
			[&](server_client& client, const char* ip)
			{
			client._uid = next_uid;
			next_uid++;
			};

	network_server.start_listening(enetpp::server_listen_params<server_client>()
										   .set_max_client_count(20)
										   .set_channel_count(1)
										   .set_listen_port(8888)
										   .set_initialize_client_function(init_client_func));

	return;
}

void Server::run()
{
	auto on_connect = ([&](server_client& client)
	{
		Printer() << "client " <<
		std::to_string(client.get_id()) <<
		": has connected" << std::endl;
	});

	auto on_disconnect = ([&](unsigned int client_uid)
	{
		Printer() << "client " <<
		std::to_string(client_uid) << ": has disconnected\n";
	});


	auto on_data = ([&](server_client& client, const enet_uint8* data, size_t data_size)
    {
		std::string msg(reinterpret_cast<const char *>(data), data_size);
		Printer() << "client " <<
				  std::to_string(client.get_id()) << ": " <<
				  msg;

		Printer() << std::endl;
		Printer() << "forwarding msg to all clients \n";

		auto predicate = [&](const server_client &destination)
        {
            return destination.get_id() != client.get_id();
        };

		network_server.send_packet_to_all_if(
				0,
				data, data_size, ENET_PACKET_FLAG_RELIABLE,
				predicate);
		});

	// while server should not terminate
	static bool terminate = false;
	while (!terminate)
	{
		network_server.consume_events(on_connect, on_disconnect, on_data);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return;
}