#include <string>
#include <vector>

#include "Client.h"
#include "Server.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	std::vector<std::string> parameter_list(argv + 1, argv + argc);
	auto server_param_iterator = std::find(parameter_list.begin(), parameter_list.end(), "-server");
	if(server_param_iterator != parameter_list.end())
	{
		auto server_name_iterator = std::next(server_param_iterator, 1);
		auto server_port_it = std::next(server_param_iterator, 2);

		std::string server_name = *server_name_iterator;
		std::string server_port_string = *server_port_it;
		size_t server_port = Utils::string_to_size_t(server_port_string);

		std::unique_ptr<Server> server(new Server(server_name, server_port));
		server->start();
	}
	auto client_param_iterator = std::find(parameter_list.begin(), parameter_list.end(), "-client");
	if (client_param_iterator != parameter_list.end())
	{
		auto username_iterator = std::next(client_param_iterator, 1);
		auto server_ip_iterator = std::next(client_param_iterator, 2);
		auto server_port_iterator = std::next(client_param_iterator, 2);

		std::string username = *username_iterator;
		std::string server_ip = *server_ip_iterator;
		std::string server_port_string = *server_port_iterator;
		size_t server_port = Utils::string_to_size_t(server_port_string);

		std::unique_ptr<Client> client(new Client(server_port, server_ip, username));
		client->connect_and_auth();
		client->start_reciver();
	}
}