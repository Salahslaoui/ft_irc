#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

int client_invited(channel &channel, client_info *client)
{
    for (int i = 0; i < channel.invited.size(); ++i)
    {
        if (channel.invited[i].nickname == client->nickname)
        {
            channel.invited.erase(channel.invited.begin() + i, channel.invited.begin() + i + 1);
            return (1);
        }
    }
    return (0);
}

int max_clients(channel &channel)
{
    if (channel.clients.size() + 1 > channel.max_clients)
        return (1);
    return 0;
}

// std::string numeric_replyy(const std::string& server_name,
//                           const std::string& code,
//                           const std::string& nick,
//                           const std::string& middle,   // for 353: symbol + channel
//                           const std::string& message)  // for 353: user list
// {
//     return ":" + server_name + " " + code + " " + nick + " " + middle + " :" + message + "\r\n";
// }

// send numeric to a client
// void send_numericc(client_info* client,
//                   const std::string& server_name,
//                   const std::string& code,
//                   const std::string& middle,
//                   const std::string& message)
// {
//     std::string msg = numeric_replyy(server_name, code, client->nickname, middle, message);
//     send(client->fd, msg.c_str(), msg.size(), 0);
// }

void send_azabi(client_info* client, const std::string& message)
{
	send(client->fd, message.c_str(),message.size(), 0);
}

std::string build_names_list(const channel& chan)
{
    std::string list;

    for (size_t i = 0; i < chan.clients.size(); ++i)
    {
        const client_info& c = chan.clients[i];

        // prefix for operator or voiced
        std::string prefix = "";
        for (size_t j = 0; j < chan.moderators.size(); ++j)
        {
            if (chan.moderators[j].nickname == c.nickname)
            {
                prefix = "@"; // operator
                break;
            }
        }
        // if you have voiced users, add "+" here

        if (i != 0)
            list += " "; // space separator
        list += prefix + c.nickname;
    }

    return list;
}

void    create_channel(std::deque<channel> &channels, std::map<std::string, std::string>::iterator it, client_info *client, std::string &tmp)
{
    if (tmp != "")
    {
        it->second = tmp;
        tmp = "";
    }
    if (channels.size() == 0)
    {
        channel add;
        add.name = it->first;
        add.key = it->second;
        add.clients.push_back(*client);
        add.moderators.push_back(*client);
        channels.push_back(add);
	
    	std::string server_name = "irc.localhost";

    // JOIN message
    	std::string join_msg = ":" + client->nickname + "!" + client->username +
                           "@" + channels[0].get_client_ip(client->fd) +
                           " JOIN :" + add.name + "\r\n";
    	send(client->fd, join_msg.c_str(), join_msg.size(), 0);

    // (Optional) no topic yet, so skip 332

    // // NAMES reply (RPL_NAMREPLY)
	std::string user_list = build_names_list(channels[0]);
    	// send_numericc(client, server_name, RPL_NAMREPLY, "= " + channels[0].name, user_list);
		std::string POSTFIX = "\r\n";
		std::string msg = ":" + server_name + " " + RPL_NAMREPLY + " " + client->nickname + " = " + channels[0].name + " :@" + client->nickname	+ POSTFIX;
		send_azabi(client, msg);
		msg = ":" + server_name + " " + RPL_NAMREPLY + " " + client->nickname + " = " + channels[0].name + " " + ":End of NAMES list" + POSTFIX;
		send_azabi(client, msg);
    // // End of NAMES (RPL_ENDOFNAMES)
    	// send_numericc(client, server_name, RPL_ENDOFNAMES, add.name, "End of /NAMES list.");

    // 	std::cout << "channel created: " << it->first << std::endl;
    	return;
        // return (send(client->fd, m.c_str(), m.size(), 0), void());
        // return std::cout << "channel created: " << it->first << std::endl, void();
    }
    for (size_t i = 0; i < channels.size(); ++i)
    {
        if (it->first == channels[i].name)
        {
            if ((channels[i].l == 1 && channels[i].clients.size() + 1 <= channels[i].max_clients) || channels[i].l == 0)
            {
                tmp = it->second;
                for (int j = 0; j < channels[i].clients.size(); ++j)
                {     
                    if (client->nickname == channels[i].clients[j].nickname)
                        return (std::cerr << "This User already exist in this channel" << std::endl, void());
                }
                if (channels[i].i && !client_invited(channels[i], client))
                    return (send_numeric(client, ERR_INVITEONLYCHAN, "JOIN", "Invite only channel\r\n"));
                else if (channels[i].l && max_clients(channels[i]))
                    return (send_numeric(client, ERR_CHANNELISFULL, "JOIN", "the limit is reached\r\n"));
                else if (channels[i].k && it->second != channels[i].key)
                    return (send_numeric(client, ERR_BADCHANNELKEY, "JOIN", "Key is wrong\r\n"));
                else
                {
                    channels[i].clients.push_back(*client);
					std::string join_msg = ":" + client->nickname + "!" + client->username +
										"@" + channels[i].get_client_ip(client->fd) +
										" JOIN :" + channels[i].name + "\r\n";
                    channels[i].broadcast(join_msg.c_str(), *client);
					send(client->fd, join_msg.c_str(), join_msg.size(), 0);
					std::string POSTFIX = "\r\n";
    				std::string server_name = "irc.localhost";
					std::string list = build_names_list(channels[i]);
					std::string msg = ":" + server_name + " " + RPL_NAMREPLY + " " + client->nickname + " = " + channels[i].name + " " + list + POSTFIX;
					send_azabi(client, msg);
					msg = ":" + server_name + " " + RPL_ENDOFNAMES + " " + client->nickname + " = " + channels[i].name + " " + ":End of NAMES list" + POSTFIX;
					send_azabi(client, msg);
                    return (std::cout << "the client " << client->nickname << " has joined " << channels[i].name << std::endl, void());
                }
            }
            else
                return (send_numeric(client, ERR_CHANNELISFULL, "JOIN", "The Channel is full\r\n"));
        }
    }
    channel add;
    add.name = it->first;
    add.key = it->second;
    add.clients.push_back(*client);
    add.moderators.push_back(*client);
    channels.push_back(add);
    std::cout << "channel created: " << it->first << std::endl;
    std::string server_name = "irc.localhost";
	std::string list = build_names_list(add);

    // JOIN message
    std::string join_msg = ":" + client->nickname + "!" + client->username +
                        "@" + add.get_client_ip(client->fd) +
                        " JOIN :" + add.name + "\r\n";
    send(client->fd, join_msg.c_str(), join_msg.size(), 0);
	std::string POSTFIX = "\r\n";
	std::string msg = ":" + server_name + " " + RPL_NAMREPLY + " " + client->nickname + " = " + add.name + " " + list + POSTFIX;
	send_azabi(client, msg);
	msg = ":" + server_name + " " + RPL_ENDOFNAMES + " " + client->nickname + " = " + add.name + " " + ":End of NAMES list" + POSTFIX;
	send_azabi(client, msg);
    return;
}

void join(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected)
{
    std::string chan;
    std::string ky;
    std::string tmp;
    std::string tmp1;
    std::string tmp2 = "";
    int         err_flag = 0;
    int         alr = 0;
    std::map<std::string, std::string> _channel;
    if (tokens.size() > 3 || tokens.size() < 2)
		return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "JOIN", "invalid parameters\r\n"));
    chan = tokens[1];
    if (tokens.size() == 3)
        ky = tokens[2];
    else
        ky = "";
    std::stringstream ss(chan);
    std::stringstream ss2(ky);
    while (std::getline(ss, tmp, ','))
    {
        if (tmp[0] != '#' && tmp[0] != '&')
        {
            send_numeric(client_connected, ERR_NOSUCHCHANNEL, "JOIN", "No such channel\r\n");
            continue;
        }
        if (tmp.find(7) != std::string::npos || tmp.size() > 20)
        {
            send_numeric(client_connected, ERR_BADCHANNAME, "JOIN", "Invalid channel name\r\n");
            continue;
        }
        if (tmp.size() == 1)
        {
            send_numeric(client_connected, ERR_BADCHANNAME, "JOIN", "Invalid channel name\r\n");
            continue;
        }
        if (ky == "")
            _channel[tmp] = "";
        else if (std::getline(ss2, tmp1, ','))
            _channel[tmp] = tmp1;
        else
            _channel[tmp] = "";
    }
    channel add;
    for (std::map<std::string, std::string>::iterator it = _channel.begin(); it != _channel.end(); ++it)
        create_channel(channels, it, client_connected, tmp2);
    std::cout << "\n\n    ||||||||     \n" <<std::endl;
    for (int i = 0; i < channels.size(); ++i)
    {
        std::cout << channels[i].name << ": " << channels[i].key << std::endl;
        std::cout << "Clients in the channel: ";
        for (int j = 0; j < channels[i].clients.size(); ++j)
            std::cout << channels[i].clients[j].nickname << " ";
        std::cout << std::endl;
        std::cout << "Moderators in the channel" << channels[i].name << " : ";
        for (int j = 0; j < channels[i].moderators.size(); ++j)
            std::cout << channels[i].moderators[j].nickname << " ";
        std::cout << "\n\n    ||||||||     \n" <<std::endl;
    }
}