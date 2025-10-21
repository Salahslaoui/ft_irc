#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

// checks if the client is an operator in the specified channel
bool check_if_op(channel* di_channel, client_info *client)
{
    std::vector<client_info*>::const_iterator it =
        std::find(di_channel->moderators.begin(),
                  di_channel->moderators.end(),
                  client);

    return (it != di_channel->moderators.end());
}

// find the channel that has the same name in the list of channels
channel* find_channel(const std::string &channel_to_find, std::vector<channel> &channels)
{
	for (std::vector<channel>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		if (channel_to_find == it->name)
			return &(*it);
	}
	return NULL;
}

bool mode_valid(std::string str)
{

}

// MODE <channel> {[+|-]|i|t|k|o|l} [<parameter>]
void mode(std::vector<std::string> tokens, std::vector<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	// checks if it has valid args
	if (tokens.size() < 3)
	{
		std::string error = numeric_reply(ERR_NEEDMOREPARAMS, client_connected->nickname, "MODE", "Not enough parameters\n");
		send(client_connected->fd,  error.c_str(), error.size(), 0);
		return ;
	}
	// checks if the channel name exists
	channel_name = tokens[1];
	if (!(di_channel = find_channel(channel_name, channels)))
	{
		std::string error = numeric_reply(ERR_NOSUCHCHANNEL, client_connected->nickname, channel_name, "No such channel\n");
		send(client_connected->fd,  error.c_str(), error.size(), 0);
		return ;
	}
	// check if the client is an operator
	if (!check_if_op(di_channel, client_connected))
	{
		std::string error = numeric_reply(ERR_CHANOPRIVSNEEDED, client_connected->nickname, channel_name, "You're not channel operator\n");
		send(client_connected->fd,  error.c_str(), error.size(), 0);
		return ;
	}
	// check if modes are valid
	
	send(client_connected->fd, "l7waaa\n", 8, 0);
}