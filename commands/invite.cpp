#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

void invite(std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients, std::vector<std::string> tokens)
{
    channel *tmp;
    client_info * tmp1;
    if (tokens.size() < 3)
        return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "INVITE", "Invalid number of arguments"));
    if (!find_client(tokens[1], clients))
        return (send_numeric(client_connected, ERR_NOSUCHNICK, "INVITE", "The target nickname doesn’t exist or isn’t connected"));
    if (!find_channel(tokens[2], channels))
        return (send_numeric(client_connected, ERR_NOSUCHCHANNEL, "INVITE", "No such channel"));
    if (!find_client(client_connected->nickname, find_channel(tokens[2], channels)->clients))
        return (send_numeric(client_connected, ERR_NOTONCHANNEL, "INVITE", "The inviter is not in that channel"));
    if (find_client(tokens[1], find_channel(tokens[2], channels)->clients))
        return (send_numeric(client_connected, ERR_USERONCHANNEL, "INVITE", "The target user is already in that channel"));
    if (!check_if_op(find_channel(tokens[2], channels), client_connected->nickname))
        return (send_numeric(client_connected, ERR_CHANOPRIVSNEEDED, "INVITE", "The inviter isn’t an operator on that channel"));
    tmp = find_channel(tokens[2], channels);
    tmp1 = find_client(tokens[1], clients);
    tmp->invited.push_back(*tmp1);
    send_numeric(client_connected, RPL_INVITING, "INVITE", "Sent to confirm the invitation to the inviter");
	std::string msg = ":" + client_connected->nickname + "!" + client_connected->username + "@" + get_client_ipp(client_connected->fd) +
                    " INVITE " + find_client(tokens[1], clients)->nickname + " :" + find_channel(tokens[2], channels)->name + "\r\n";
	send_it_cl(client_connected, msg);
}
