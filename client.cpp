#include "includes/client.hpp"

Client::Client() : fd(-1), registred(0), username("unknown"), nickname("nickname"), realname("unknown"),f_buffer(""), pass_auth(0), user_auth(0), nick_auth(0)
{}

Client::~Client()
{}

std::string Client::get_nick()
{
    return (nickname);
}

int Client::get_pass_auth()
{
    return (pass_auth);
}

std::string Client::get_real_user()
{
    return (realname);
}

int Client::get_user_auth()
{
    return (user_auth);
}

int Client::get_nick_auth()
{
    return (nick_auth);
}

std::string Client::get_fbuffer()
{
	return (f_buffer);
}

int Client::get_regt()
{
    return (registred);
}

std::string Client::get_username()
{
    return (username);
}

int Client::get_fd()
{
    return (fd);
}

void Client::set_nick(std::string set)
{
    nickname = set;
}

void Client::set_user_auth(int set)
{
    user_auth = set;
}

void Client::set_nick_auth(int set)
{
    nick_auth = set;
}

void Client::set_fbuffer(std::string setter)
{
	f_buffer = setter;
}

void Client::set_real_name(std::string set)
{
    realname = set;
}

void Client::set_regt(int set)
{
    registred = set;
}

void Client::set_pass_auth(int set)
{
    pass_auth = set;
}

void Client::set_username(std::string set)
{
    username = set;
}

void Client::set_fd(int set)
{
    fd = set;
}
