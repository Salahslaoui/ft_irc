#include "includes/server.hpp"
#include "includes/helper.hpp"
#include "includes/channel.hpp"
#include "includes/client.hpp"

int dup_nick(std::string name, std::vector<Client> clients)
{
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i].get_nick() == name)
            return (1);
    }
    return (0);
}

void send_it(Client client, std::string str)
{
	if (*(client.get_revent()) & POLLOUT)
		send(client.get_fd(), str.c_str(),str.size(), 0);
}



void    other_prec(std::deque<channel> &channels, std::string str, Client *client)
{
    if (str == client->get_nick())
        return;
    for (size_t i = 0; i < channels.size(); ++i)
    {
        for (size_t j = 0; j < channels[i].clients.size();)
        {
            if (client->get_nick() == channels[i].clients[j].nickname)
                channels[i].clients[j].nickname = str;
            else
                j++;
        }
        for (size_t j = 0; j < channels[i].moderators.size();)
        {
            if (client->get_nick() == channels[i].moderators[j].nickname)
                channels[i].moderators[j].nickname = str;
            else
                j++;
        }
        for (size_t j = 0; j < channels[i].invited.size();)
        {
            if (client->get_nick() == channels[i].invited[j].nickname)
                channels[i].invited[j].nickname = str;
            else
                j++;
        }
    }
}

int auth(std::vector<std::string> tokens, Client *client, std::vector<Client> &clients, std::string s_pass, std::deque<channel> &channels)
{
	std::string str = "";
	if (tokens[0] == "PASS" || tokens[0] == "pass")
	{
		if (tokens.size() < 2)
		{
			str += ":ircserv ";
			str += ERR_NEEDMOREPARAMS;
			str += " * :Invalid numbers of arguments\r\n";
			return (send_it(*client, str), 0);
		}
		else if (tokens[1] != s_pass)
		{
			str += ":ircserv ";
			str += ERR_PASSWDMISMATCH;
			str += " * :Password incorrect\r\n";
			return (send_it(*client, str), 0);
		}
		else if (client->get_pass_auth())
		{
			str += ":ircserv ";
			str += ERR_ALREADYREGISTERED;
			str += " * :Already registred\r\n";
			return (send_it(*client, str), 0);
		}
		else
		{
			for (size_t i = 0; i < clients.size(); ++i)
			{
				if (client->get_fd() == clients[i].get_fd())
					clients[i].set_pass_auth(1);
			}
		}
	}

    if (tokens[0] == "NICK" || tokens[0] == "nick")
    {
        if (tokens.size() < 2)
        {
            str += ":ircserv ";
            str += ERR_NEEDMOREPARAMS;
			str += " * :Invalid numbers of arguments\r\n";
            return (send_it(*client, str), 0);
        }
        else if (client->get_pass_auth() == 0)
        {
            str += ":ircserv ";
            str += ERR_NOTREGISTERED;
            str += " * : You have not registered\r\n";
            return (send_it(*client, str), 0);
        }
        // else if (client->get_nick_auth())
        // {
		// 	str += ":ircserv ";
		// 	str += ERR_ALREADYREGISTERED;
		// 	str += " * :Already registred\r\n";
		// 	return (send(client->get_fd(), str.c_str(), str.size(), 0), 0);
        // }
        else if (dup_nick(tokens[1], clients))
        {
            str += ":ircserv ";
            str += ERR_NICKNAMEINUSE;
            str += " * ";
            str += tokens[1];
            str += " : Nickname is already in use\r\n";
            return (send_it(*client, str), 0);
        }
        else
        {
            other_prec(channels, tokens[1], client);
            client->set_nick(tokens[1]);
            client->set_nick_auth(1);
            return (0);
        }
    }

    if (tokens[0] == "USER" || tokens[0] == "user")
    {
        if (client->get_pass_auth() == 0)
        {
            str += ":ircserv ";
            str += ERR_NOTREGISTERED;
            str += " * : You have not registered\r\n";
            return (send_it(*client, str), 0);
        }
        else if (tokens.size() < 5)
        {
            str += ":ircserv ";
            str += ERR_NEEDMOREPARAMS;
            str += " : Invalid parameters\r\n";
            return (send_it(*client, str), 0);
        }
        else if (client->get_user_auth())
        {
			str += ":ircserv ";
			str += ERR_ALREADYREGISTERED;
			str += " * :Already registred\r\n";
			return (send_it(*client, str), 0);
        }
        else if (client->get_nick_auth() == 0)
        {
            str += ":ircserv ";
            str += ERR_NONICKNAMEGIVEN;
            str += " * : You must specify your nickname before user\r\n";
            return (send_it(*client, str), 0);
        }
        else
        {
            client->set_username(tokens[1]);
            client->set_user_auth(1);
            client->set_real_name(tokens[4]);
        }
    }
    if (client->get_pass_auth() && client->get_user_auth() && client->get_nick_auth())
    {
        client->set_regt(1);
        std::cout << "Client registred" << std::endl;
        return (client->get_regt());
    }
    return (client->get_regt());
}

std::vector<std::string> split(std::string buffer)
{
	std::vector<std::string> tmp;
	std::stringstream sp(buffer);
	std::string line;

	while (std::getline(sp, line, ' '))
		tmp.push_back(trim(line));
	return tmp;
}

client_info converter(Client *client)
{
    client_info tmp;

    tmp.username = client->get_username();
    tmp.nickname = client->get_nick();
    tmp.fd = client->get_fd();
    tmp.has_register = client->get_regt();
	tmp.poll_check = client->get_revent();
    return (tmp);
}

void Commands(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients)
{
	for (int i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << " ";
	std::cout << std::endl;
	if (tokens[0] == "JOIN" || tokens[0] == "join")
        join(tokens, channels, client_connected);
    else if (tokens[0] == "MODE" || tokens[0] == "mode")
        mode(tokens, channels, client_connected);
    else if (tokens[0] == "PRIVMSG" || tokens[0] == "privmsg")
        privmsg(tokens, channels, client_connected, clients);
    else if (tokens[0] == "TOPIC" || tokens[0] == "topic")
        topic(tokens, channels, client_connected);
    else if (tokens[0] == "INVITE" || tokens[0] == "invite")
        invite(channels, client_connected, clients, tokens);
    else if (tokens[0] == "KICK" || tokens[0] == "kick")
        kick(tokens, channels, client_connected);
	else if (tokens[0] == "PING")
	{
		if (tokens.size() > 1)
		{
			std::string pong = "PONG " + tokens[1] + "\r\n";
			send_it_cl(client_connected, pong);
		}
	}
    // else if (tokens[0] == "USER")
    // {
    //     send_numeric_user(client_connected, "462");
    // }
    else if (tokens[0] == "PONG")
        return;
    else
        send_numeric(client_connected, "421", tokens[0], "Unknown command");
}

void server_info::handle_auth(std::string buffer, Client *client_connected, std::vector<Client> &clients)
{
	std::vector<std::string> tokens;

	tokens = split(buffer);
    if (tokens.empty())
        return ;

	if (!auth(tokens, client_connected, clients, server_password, channels))
		return ;
    // if (tokens[0] == "USER" || tokens[0] == "user")
    //     return ;
    Cl = converter(client_connected);
    clients_s.clear();
    for (size_t i = 0; i < clients.size(); ++i)
    {
        client_info tmp = converter(&clients[i]);
        clients_s.push_back(tmp);
    }
    if (tokens[0] == "PASS" || tokens[0] == "NICK" || tokens[0] == "USER"
        || tokens[0] == "pass" || tokens[0] == "nick" || tokens[0] == "user")
        return;
    Commands(tokens, channels, &Cl, clients_s);
}
