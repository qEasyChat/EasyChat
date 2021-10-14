#include "Utils.h"

#include <fstream>
#include <iostream>
#include <sstream>

void Utils::memory_error()
{
	std::cerr << "MEMORY ERROR: server connection" << std::endl;
	exit(EXIT_FAILURE);
}

size_t Utils::string_to_size_t(std::string string)
{

	std::stringstream sstream(string);
	size_t result;
	sstream >> result;
	return result;
}

std::string Utils::get_query(std::string file)
{
	std::string query = "";
	std::fstream query_file(file);
	std::string line;
	while (getline(query_file, line)) {
		query += line + " ";
	}

	return query;
}
