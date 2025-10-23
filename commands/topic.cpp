#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

void topic(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	std::string modes;
	bool add = true;
	int args_start = 3;

	
	// checks if it has valid args
	if (tokens.size() < 2)
		return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "TOPIC", "Not enough parameters\n"));

	// checks if the channel name exists
	channel_name = tokens[1];
	if (!(di_channel = find_channel(channel_name, channels)))
		return (send_numeric(client_connected, ERR_NOSUCHCHANNEL, channel_name, "No such channel\n"));

	// check if the client is a member in the channel
	if (!find_client(client_connected->nickname, di_channel->clients))
		return (send_numeric(client_connected, ERR_NOTONCHANNEL, channel_name, "You're not on that channel\n"));
	
		// check if the client is an operator
	if (!di_channel->t && !check_if_op(di_channel, client_connected->nickname) && tokens.size() != 2)
		return (send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, channel_name, "You're not channel operator\n"));

}