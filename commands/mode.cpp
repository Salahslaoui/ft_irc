#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

bool modify_channel_op(channel* di_channel, std::string Client_to_add, bool to_add, client_info *client_connected)
{
	client_info *client = find_client(Client_to_add, di_channel->clients);

	if (!client)
		return (send_numeric(client_connected, ERR_USERNOTINCHANNEL, Client_to_add + " " +di_channel->name, "They aren't on that channel\n"), false);
	if (to_add)
	{
		if (!check_if_op(di_channel, Client_to_add))
			di_channel->moderators.push_back(*client);
	}
	else
	{
		if (di_channel->moderators.size() == 1)
    		return (send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, di_channel->name, "Cannot remove the only channel operator\n"), false);
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
	return true;
}

// MODE <channel> {[+|-]|i|t|k|o|l} [<parameter>]
void mode(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	std::string modes;
	bool add = true;
	int args_start = 3;

	// Use a vector of pairs to preserve insertion order
	std::vector<std::pair<char, std::string> > valid_modes;

	// checks if it has valid args
	if (tokens.size() < 3)
		return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n"));

	// checks if the channel name exists
	channel_name = tokens[1];
	if (!(di_channel = find_channel(channel_name, channels)))
		return (send_numeric(client_connected, ERR_NOSUCHCHANNEL, channel_name, "No such channel\n"));

	// check if the client is a member in the channel
	if (!find_client(client_connected->nickname, di_channel->clients))
		return (send_numeric(client_connected, ERR_NOTONCHANNEL, channel_name, "You're not on that channel\n"));
	
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
			valid_modes.push_back(std::make_pair(modes[i], ""));
			continue;
		}
		else if (modes[i] == '-')
		{
			add = false;
			valid_modes.push_back(std::make_pair(modes[i], ""));
			continue;
		}
		else if (modes[i] == 'i')
		{
			di_channel->i = add;
			valid_modes.push_back(std::make_pair(modes[i], ""));
		}
		else if (modes[i] == 't')
		{
			di_channel->t = add;
			valid_modes.push_back(std::make_pair(modes[i], ""));
		}
		else if (modes[i] == 'k')
		{
			if (add)
			{
				if (args_start >= tokens.size())
				{
					send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n");
					continue;
				}
				valid_modes.push_back(std::make_pair(modes[i], tokens[args_start]));
				di_channel->k = add;
				di_channel->key = tokens[args_start++];
			}
			else
			{
				valid_modes.push_back(std::make_pair(modes[i], ""));
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
			bool flag = modify_channel_op(di_channel, tokens[args_start], add, client_connected);
			if (flag)
				valid_modes.push_back(std::make_pair(modes[i], tokens[args_start++]));
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

				std::string limit_str = tokens[args_start];
				long long_limit = 0;
				bool valid = true;

				for (size_t j = 0; j < limit_str.size(); j++)
				{
					if (!isdigit(limit_str[j]))
					{
						valid = false;
						break;
					}
				}

				if (!valid)
				{
					send_numeric(client_connected, ERR_UNKNOWNMODE, "l", "Invalid user limit\n");
					continue;
				}

				long_limit = atol(limit_str.c_str());
				if (long_limit > INT_MAX)
				{
					send_numeric(client_connected, ERR_UNKNOWNMODE, "l", "User limit too big\n");
					continue;
				}

				if (long_limit <= 0)
				{
					send_numeric(client_connected, ERR_UNKNOWNMODE, "l", "User limit must be positive\n");
					continue;
				}

				valid_modes.push_back(std::make_pair(modes[i], tokens[args_start++]));
				di_channel->l = true;
				di_channel->max_clients = static_cast<int>(long_limit);
			}
			else
			{
				valid_modes.push_back(std::make_pair(modes[i], ""));
				di_channel->l = false;
				di_channel->max_clients = 0;
			}
		}
		else
			send_numeric(client_connected, ERR_UNKNOWNMODE, std::string(1, modes[i]), " is unknown mode char to me\n");
	}

	// ✅ Broadcast the mode change to everyone in the channel
	std::string broadcast_msg = ":" + client_connected->nickname + "!~" + client_connected->username +
	                            "@IRC MODE " + di_channel->name + " ";

	// First loop → append all the mode characters in order
	for (size_t i = 0; i < valid_modes.size(); ++i)
	{
		if ((valid_modes[i].first == '+' || valid_modes[i].first == '-') && (i + 1 < valid_modes.size() || valid_modes[i + 1].first == '+' || valid_modes[i + 1].first == '-'))
			continue;
		broadcast_msg += valid_modes[i].first;
	}

	// Second loop → append all the arguments
	for (size_t i = 0; i < valid_modes.size(); ++i)
	{
		if (!valid_modes[i].second.empty())
			broadcast_msg += " " + valid_modes[i].second;
	}

	std::cout << broadcast_msg << std::endl;
	di_channel->broadcast(broadcast_msg, *client_connected, false);
}
