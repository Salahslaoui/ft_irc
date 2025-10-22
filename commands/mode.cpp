#include "../includes/channel.hpp"
#include "../includes/helper.hpp"


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


void modify_channel_op(channel* di_channel, std::string Client_to_add, bool to_add, client_info *client_connected)
{
	client_info *client = find_client(Client_to_add, di_channel->clients);

	if (!client)
		return (send_numeric(client_connected, ERR_USERNOTINCHANNEL, Client_to_add + di_channel->name, ":They aren't on that channel\n"));
	if (to_add)
	{
		if (!check_if_op(di_channel, Client_to_add))
			di_channel->moderators.push_back(*client);
	}
	else
	{
		if (di_channel->moderators.size() == 1)
    		return send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, di_channel->name, "Cannot remove the only channel operator\n");
		for (std::vector<client_info>::iterator it = di_channel->moderators.begin();
			it != di_channel->moderators.end(); ++it)
		{
			if (it->nickname == client->nickname)
			{
				di_channel->moderators.erase(it);
				break;
			}
		}
	}
}



// MODE <channel> {[+|-]|i|t|k|o|l} [<parameter>]
void mode(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	std::string modes;
	bool add = true;
	int args_start = 3;
	// checks if it has valid args
	if (tokens.size() < 3)
		return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n"));

	// checks if the channel name exists
	channel_name = tokens[1];
	if (!(di_channel = find_channel(channel_name, channels)))
		return (send_numeric(client_connected, ERR_NOSUCHCHANNEL, channel_name, "No such channel\n"));

	// check if the client is an operator
	if (!check_if_op(di_channel, client_connected->nickname))
		return (send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, channel_name, "You're not channel operator\n"));

	// check if modes are valid
	modes = tokens[2];
	if (tokens[2].empty() || (tokens[2][0] != '+' && tokens[2][0] != '-')) 
		return (send_numeric(client_connected, ERR_UNKNOWNMODE, std::string(1,tokens[2][0]), "is unknown mode char to me\n"));

	for (int i = 0; i < modes.size(); i++)
	{
		if (modes[i] == '+')
		{
			add = true;
			continue;
		}
		if (modes[i] == '-')
		{
			add = false;
			continue;
		}
		if (modes[i] ==  'i')
			di_channel->i = add;
		else if (modes[i] == 't')
			di_channel->t = add;
		else if (modes[i] == 'k')
		{
			if (add)
			{
				if (args_start >= tokens.size())
				{
					send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n");
					continue;
				}
				di_channel->k = add;
				di_channel->key = tokens[args_start++];
			}
			else
			{
				di_channel->k = add;
				di_channel->key = "";
			}
		}
		else if (modes[i] == 'o')
		{
			if (args_start >= tokens.size())
			{
				send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n");
				continue;
			}
			modify_channel_op(di_channel, tokens[args_start++], add, client_connected);
		}
		else if (modes[i] == 'l')
		{
			if (add)
			{
				if (args_start >= tokens.size())
				{
					send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n");
					continue;
				}
				di_channel->l = add;
				// di_channel->max_clients = 
			}
		}
	}

	send(client_connected->fd, "l7waaa\n", 8, 0);
}
