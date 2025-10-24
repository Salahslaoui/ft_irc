#include "../includes/helper.hpp"

std::string trim(const std::string& s)
{
	if (s.empty()) return "";
    size_t start = s.find_first_not_of("\r\n\t ");
    size_t end = s.find_last_not_of("\r\n\t ");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

std::string numeric_reply(const std::string& code, const std::string& nick, const std::string& command, const std::string& message)
{
    return ":IRC " + code + " " + nick + " " + command + " :" + message + "\r\n";
}

// helper function to send a numeric reply to a client
void send_numeric(client_info* client, const std::string& code,
                  const std::string& target, const std::string& message)
{
    std::string error = numeric_reply(code, client->nickname, target, message);
    send(client->fd, error.c_str(), error.size(), 0);
}

// find the channel that has the same name in the list of channels
channel* find_channel(const std::string &channel_to_find, std::deque<channel> &channels)
{
	for (std::deque<channel>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		if (channel_to_find == it->name)
			return &(*it);
	}
	return NULL;
}

// find the client that has the same name in the list of clients
client_info* find_client(const std::string &client_to_find, std::vector<client_info> &clients)
{
	for (std::vector<client_info>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (client_to_find == it->nickname)
			return &(*it);
	}
	return NULL;
}

// checks if the client is an operator in the specified channel
client_info* check_if_op(channel* di_channel, std::string client)
{
	for (std::vector<client_info>::iterator it = di_channel->moderators.begin(); it != di_channel->moderators.end(); it++)
	{
		if (client == it->nickname)
			return &(*it);
	}
	return NULL;
}
