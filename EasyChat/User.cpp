#include "User.h"

User::~User()
{
	this->id = 0;
	this->username = "";
	this->password_hash = "";
}


size_t User::get_id() const
{
	return id;
}

void User::set_id(const size_t id)
{
	this->id = id;
}

std::string User::get_username() const
{
	return username;
}

void User::set_username(const std::string& username)
{
	this->username = username;
}

std::string User::get_password_hash() const
{
	return password_hash;
}

void User::set_password_hash(const std::string& password_hash)
{
	this->password_hash = password_hash;
}
