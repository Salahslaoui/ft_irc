#include "includes/server.hpp"

int auth(std::vector<std::string> tokens, Client client, std::vector<Client> &clients, std::string s_pass)
{
	std::string str = "";
	if (tokens[0] == "PASS" || tokens[0] == "pass")
	{
		if (tokens.size() < 2)
		{
			str += ":ircserv ";
			str += ERR_NEEDMOREPARAMS;
			str += " * :Invalid numbers of arguments\r\n";
			return (send(client.get_fd(), str.c_str(), str.size(), 0), 0);
		}
		else if (tokens[1] != s_pass)
		{
			str += ":ircserv ";
			str += ERR_PASSWDMISMATCH;
			str += " * :Password incorrect\r\n";
			return (send(client.get_fd(), str.c_str(), str.size(), 0), 0);
		}
		else if (client.get_regt())
		{
			str += ":ircserv ";
			str += ERR_ALREADYREGISTERED;
			str += " * :Already registred\r\n";
			return (send(client.get_fd(), str.c_str(), str.size(), 0), 0);
		}
		else
		{
			for (size_t i = 0; i < clients.size(); ++i)
			{
				if (client.get_fd() == clients[i].get_fd())
					clients[i].set_pass_auth(1);
			}
		}
	}

    // if (tokens[0] == "USER" || tokens[0] == "user")
}

std::vector<std::string> split(std::string buffer)
{
	std::vector<std::string> tmp;
	std::stringstream sp(buffer);
	std::string line;

	while (std::getline(sp, line, ' '))
		tmp.push_back(line);
	return tmp;
}

void	handle_auth(std::string buffer, Client client_connected, std::vector<Client> &clients, std::string s_pass)
{
	std::vector<std::string> tokens;
	tokens = split(buffer);
	if (!auth(tokens, client_connected, clients, s_pass))
		return ;
}
