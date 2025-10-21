#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

// helper function to send a numeric reply to a client
void send_numeric(client_info* client, const std::string& code,
                  const std::string& target, const std::string& message)
{
    std::string error = numeric_reply(code, client->nickname, target, message);
    send(client->fd, error.c_str(), error.size(), 0);
}

// checks if the client is an operator in the specified channel
// bool check_if_op(channel* di_channel, client_info *client)
// {
//     // Print all moderators
// 	exit(0);
//     std::cout << "Moderators in channel " << di_channel->name << ": ";
//     for (std::vector<client_info>::const_iterator it = di_channel->moderators.begin();
//          it != di_channel->moderators.end(); ++it)
//     {
//         std::cout << (it)->nickname << " ";
//     }
//     std::cout << std::endl;

//     // Check if the given client is an operator
//     std::vector<client_info>::const_iterator it =
//         std::find(di_channel->moderators.begin(),
//                   di_channel->moderators.end(),
//                   client);

//     return (it != di_channel->moderators.end());
// }

bool check_if_op(channel* di_channel, client_info* client)
{
    for (std::vector<client_info>::const_iterator it = di_channel->moderators.begin();
         it != di_channel->moderators.end(); ++it)
    {
        if (it->nickname == client->nickname) // simple comparison
            return true;
    }
    return false;
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

// MODE <channel> {[+|-]|i|t|k|o|l} [<parameter>]
void mode(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
	std::string channel_name;
	channel* di_channel;
	std::vector<char> valid_modes;

	// checks if it has valid args
	if (tokens.size() < 3)
		return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters\n"));

	// checks if the channel name exists
	channel_name = tokens[1];
	if (!(di_channel = find_channel(channel_name, channels)))
		return (send_numeric(client_connected, ERR_NOSUCHCHANNEL, channel_name, "No such channel\n"));

	// check if the client is an operator
	if (!check_if_op(di_channel, client_connected))
		return (send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, channel_name, "You're not channel operator\n"));

	// check if modes are valid
	std::string allowed = "itkol";
	if (tokens[2].empty() || (tokens[2][0] != '+' && tokens[2][0] != '-')) 
		return (send_numeric(client_connected, ERR_UNKNOWNMODE, std::string(1,tokens[2][0]), "is unknown mode char to me\n"));

	// push the + / -
	valid_modes.push_back(tokens[2][0]);

	// push valid modes to vec and send error for unvalid ones
	for (int i = 1; i < tokens[2].size(); i++)
	{
		if (allowed.find(tokens[2][i]) == std::string::npos)
			send_numeric(client_connected, ERR_UNKNOWNMODE, std::string(1,tokens[2][i]), "is unknown mode char to me\n");
		else
			valid_modes.push_back(tokens[2][i]);
	}

	send(client_connected->fd, "l7waaa\n", 8, 0);
}
