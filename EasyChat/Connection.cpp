#include "Connection.h"

#include <iomanip>
#include <iostream>
#include <sstream>

Connection::Connection(int port_number, const std::string ip, const std::string username = "Anon")
    : port_number(port_number),
    username(username),
    ip(ip)
{
    if (socket_init() != 0) {
        std::cerr << ("socket init failed") << std::endl;
    }
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
}

Connection::~Connection()
{
    socket_close();
    this->sock = 0;
    this->username = "";
    this->port_number = 0;
}

std::string Connection::get_fixed_length_size(std::string message)
{
	std::stringstream stream;
	stream << std::setw(SIZE_BYTES) << std::setfill('0') << message.size();
	std::string size = stream.str();
	return size;
}

void Connection::send_message(std::string message)
{
    std::string encapsulated_string = get_fixed_length_size(message) + message;
    size_t total_bytes_sent = 0;
    size_t bytes_sent = 0;
    while (total_bytes_sent < encapsulated_string.size()) {
        std::string message_left = encapsulated_string.substr(bytes_sent);
        bytes_sent = send(this->sock, message_left.c_str(), message_left.size(), 0);
        total_bytes_sent += bytes_sent;
        if (bytes_sent < 0) {
            throw Message_Not_Sent_Exception();
        }
    }
}

std::string Connection::recive_message()
{
    std::string message = "";
    std::string recv_string = get_message(SIZE_BYTES);
    size_t message_size = get_size_from(recv_string);
    if (message_size > 0) {
        message = get_message(message_size);
    }
    return message;
}

size_t Connection::get_size_from(std::string fixed_length_string)
{
    fixed_length_string.erase(0, std::min(fixed_length_string.find_first_not_of('0'), fixed_length_string.size() - 1));
    size_t size;
    if (fixed_length_string.size() < 1) {
        return 0;
    }
    try {
        size = std::stol(fixed_length_string);
    }
    catch (const std::exception& e) {
        std::cerr << "connection: " << e.what() << " " << fixed_length_string << '\n';
    }
    return size;
}

std::string Connection::get_message(size_t size)
{
    std::string message = "";
    std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE]);
    size_t bytes_recived = 0;
    while (bytes_recived < size) {
        memset(buffer.get(), '\0', BUFFER_SIZE);
        size_t len = recv(this->sock, buffer.get(), size - bytes_recived, 0);
        if (len < 0) {
            throw Socket_Error_Exception();
        }
        if (len == 0) {
            throw Client_Down_Exception();
        }
        std::string recived_message = std::string(buffer.get());
        message += recived_message;
        bytes_recived += len;
    }
    return message;

}

int Connection::socket_init() {
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(1, 1), &wsa_data);
}

int Connection::socket_close() {
    int status = 0;
    status = shutdown(this->sock, SD_BOTH);
    if (status == 0) {
        status = closesocket(this->sock);
    }
    return status;
}

SOCKET Connection::get_socket()
{
	return this->sock;
}

void Connection::set_socket(SOCKET socket)
{
	this->sock = socket;
}

int Connection::get_port_number()
{
	return this->port_number;
}

void Connection::set_port_number(int port_number)
{
	this->port_number = port_number;
}

std::string Connection::get_username()
{
	return this->username;
}

void Connection::set_username(std::string username)
{
	this->username = username;
}

std::string Connection::get_ip()
{
    return this->ip;
}

void Connection::set_ip(std::string ip)
{
    this->ip= ip;
}