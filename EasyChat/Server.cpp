#include "Server.h"

#include <iostream>

#include "Utils.h"

Server::Server(std::string name, size_t port) {
	this->name = name;
	
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	this->server_connection = std::unique_ptr<Connection>(new Connection(port, LOOPBACK_ADDR, "SERVER"));
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_number);

	InetPton(AF_INET, (PCWSTR)LOOPBACK_ADDR, &server_addr.sin_addr.s_addr);

	client_addr_size = sizeof(client_addr);

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
		std::cerr << "binding uunsuccessful" << std::endl;
		exit(1);
	}

	if (listen(server_sock, 5) != 0) {
		std::cerr << "listening unsuccessful" << std::endl;
		exit(1);
	}
}

void Server::start() {
	while (1) {
		SOCKET client_sock;
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
		
		mtx.lock();
		PSTR ip_PSTR;
		inet_ntop(AF_INET, (struct sockaddr*)&client_addr, ip_PSTR, INET_ADDRSTRLEN);
		std::string ip = std::string(ip_PSTR);
		std::cout << "connection successful " << ip << std::endl;
		std::unique_ptr<Connection> client_connection = std::unique_ptr<>(new Connection());
		if (client_connection == nullptr)
		{
			Utils::memory_error();
		}
		client_connection->set_socket(client_sock);
		std::string username = client_connection->recive_message();
		client_connection->set_username(username);

		client_connection->send_message(this->name);

		std::string login_message;
		if (true)
		{
			login_message = "OK";
		} else
		{
			login_message = "RETRY";
		}

		client_connection->send_message(login_message);
		if (login_message == "OK") {
			this->username_connection_map[username] = client_connection;
			std::thread worker(&Server::recv_msg, this, client);
			workers[client_sock] = std::move(worker);
		}

		mtx.unlock();
	}
	for (auto client : clients) {
		if (workers[client.get_socket_number()].joinable()) {
			workers[client.get_socket_number()].join();
		}
	}
}

void Server::reciver(std::shared_ptr<Connection> client_connection) {
	std::string username = client_connection->get_username();
	std::cout << "reciver started for " << username << std::endl;
	std::string message;
	try {
		while (message != "SOCKET_DOWN") {
			message = client_connection->recive_message();
			send_to_all(message);
		}
	}
	catch (Client_Down_Exception exception) {
		std::cout << username << " has disconnected" << std::endl;
		this->remove_user(client_connection);
	}
}

void Server::remove_user(std::shared_ptr<Connection> connection)
{
	mtx.lock();
	std::string username = connection->get_username();
	this->username_connection_map.erase(username);
	this->workers.erase(username);
	mtx.unlock();

}