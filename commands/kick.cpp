#include "../includes/channel.hpp"
#include "../includes/helper.hpp"


void kick_client(channel &kick_channel, std::string client_to_kick)
{
	client_info *client = find_client(client_to_kick, kick_channel.clients);

	for (std::vector<client_info>::iterator it = kick_channel.clients.begin();
			it != kick_channel.clients.end(); ++it)
		{
			if (it->nickname == client->nickname)
			{
				kick_channel.clients.erase(it);
				break;
			}
		}
}

void kick(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	size_t index;
	
	// checks if it has valid args
	if (tokens.size() < 3 || (tokens.size() == 3 && tokens[2][0] == ':' && tokens[2].size() == 1))
		return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "KICK", "Not enough parameters\n"));

	// checks if the channel name exists
	channel_name = tokens[1];
	if (!(di_channel = find_channel(channel_name, channels)))
		return (send_numeric(client_connected, ERR_NOSUCHCHANNEL, channel_name, "No such channel\n"));

	// check if the client is a member in the channel
	if (!find_client(client_connected->nickname, di_channel->clients))
		return (send_numeric(client_connected, ERR_NOTONCHANNEL, channel_name, "You're not on that channel\n"));
	
	if (!find_client(tokens[2], di_channel->clients))
		return (send_numeric(client_connected, ERR_USERNOTINCHANNEL, tokens[2] + " " + channel_name, "They aren't on that channel\n"));

	// check if the client is an operator
	if (!check_if_op(di_channel, client_connected->nickname))
		return (send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, channel_name, "You're not channel operator\n"));

	if (client_connected->nickname == tokens[2])
	{
		send_numeric(client_connected, ERR_UNKNOWNCOMMAND, "KICK", "You cannot kick yourself\n");
		return;
	
	}
	std::string msg = ":" + client_connected->nickname + "!" + client_connected->username +
                  "@" + di_channel->get_client_ip(client_connected->fd) + " KICK " + channel_name + " " + tokens[2] + " ";
	for (size_t i = 3; i < tokens.size() - 1; i++)
	{
		msg += tokens[i] + " ";
		index = i;
	}
	if (index + 1 < tokens.size())
		msg += tokens[++index];

	di_channel->broadcast(msg, *client_connected, false);
	kick_client(*di_channel, tokens[2]);
}