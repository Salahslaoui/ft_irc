#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

void set_topic(channel &topic_channel, std::vector<std::string> tokens)
{
	std::string new_topic;

	if (tokens[2][0] == ':')
		new_topic = tokens[2].substr(1, tokens[2].size() - 1) + " ";
	else
		new_topic = tokens[2] + " ";

	for (int i = 3; i < tokens.size(); i++)
		new_topic += tokens[i] + " ";
	topic_channel.topic = new_topic; 
}

void topic(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	std::string modes;
	bool add = true;
	int args_start = 3;

	
	// checks if it has valid args
	if (tokens.size() < 2 || (tokens[1][0] == ':' && tokens[1].size() == 1))
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

	if (tokens.size() == 2)
	{
		if (!(di_channel->topic_flag))
			return (send_numeric(client_connected, RPL_TOPIC, channel_name, di_channel->topic));
		else
			return (send_numeric(client_connected, RPL_NOTOPIC, channel_name, "No topic is set\n"));
	}
	else
	{
		set_topic(*di_channel, tokens);
		std::string broadcast_msg = ":" + client_connected->nickname + "!~" + client_connected->username +
	                            "@" + di_channel->get_client_ip(client_connected->fd) + " TOPIC " + di_channel->name + " :" + di_channel->topic;
		di_channel->broadcast(broadcast_msg, *client_connected, false);
	}
}