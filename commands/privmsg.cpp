#include "../includes/channel.hpp"
#include "../includes/helper.hpp"

std::string parse_msg(std::vector<std::string> tokens)
{
    std::string tmp = "";

    if (tokens[2][0] != ':')
        return (tokens[2] + "\r\n");
    if (tokens[2][0] == ':')
    {
        tmp += tokens[2].substr(tokens[2].find(':') + 1);
        tmp += ' ';
    }
    for (size_t i = 3; i < tokens.size(); ++i)
    {
        tmp += tokens[i];
        tmp += ' ';
    }
    return (tmp + "\r\n");
}

void    check_and_send(std::vector<std::string> targets, std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients, std::string msg)
{
    int flag = 0;

    for(size_t i = 0; i < targets.size(); ++i)
    {
        if (targets[i][0] == '#' || targets[i][0] == '&')
        {
            for (size_t j = 0; j < channels.size(); ++j)
            {
                if (targets[i] == channels[j].name)
                {
					for (size_t k = 0; k < channels[j].clients.size(); ++k)
					{
						if (channels[j].clients[k].nickname == client_connected->nickname)
						{
							flag = 1;
							std::string broadcast_msg = ":" + client_connected->nickname + "!~" + client_connected->username +
									"@" + channels[j].get_client_ip(client_connected->fd) +  " PRIVMSG " + channels[j].name + " :" + msg;
							channels[j].broadcast(broadcast_msg, *client_connected, true);
							break;
						}
					}
					if (flag == 0)
					{
						send_numeric(client_connected, ERR_USERNOTINCHANNEL, "PRIVMSG", "The user is not in this channel");
						flag = 1;
						break;
					}
                }
            }
            if (flag == 0)
            {
                send_numeric(client_connected, ERR_NOSUCHCHANNEL, "PRIVMSG", "This channel doesn't exist");
                continue;
            }
            else
            {
                flag = 0;
                continue;
            }
        }
        for (size_t j = 0; j < clients.size(); ++j)
        {
            if (targets[i] == clients[j].nickname)
            {
                if (clients[j].has_register == 0)
                {
                    std::string str = ":ircserv ";
                    str += ERR_NOSUCHNICK;
                    str += " ";
                    str += client_connected->nickname;
                    str += " ";
                    str += targets[i];
                    str += " :No such nick/channel\r\n";
                    send_it_cl(client_connected, str);;
                    flag = 1;
                    break;
                }
				std::string broadcast_msg = ":" + client_connected->nickname + "!~" + client_connected->username +
							"@" + get_client_ipp(client_connected->fd) + " PRIVMSG " + clients[j].nickname + " :" + msg;
                send_it_cl(&clients[j], broadcast_msg);
                flag = 1;
                break;
            }
        }
        if (flag == 1)
        {
            flag = 0;
            continue;
        }
        return (send_numeric(client_connected, ERR_NOSUCHNICK, "PRIVMSG", "This client doesn't exist"));
    }
}

void    privmsg(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients)
{
    std::vector<std::string> targets;
    std::string target;
    std::string tmp;
    std::string msg;

    if (tokens.size() < 3)
        return (send_numeric(client_connected, ERR_NEEDMOREPARAMS, "PRIVMSG", "Invalid numbers of arguments"));
    if (tokens[1][0] == '#')
    {
        if (tokens[1].size() == 1)
            return (send_numeric(client_connected, ERR_BADCHANNAME, "PRIVMSG", "Invalid channel name"));
    }
    target = tokens[1];
    std::stringstream ss(target);
    while (std::getline(ss, tmp, ','))
    {
        if (tmp.size() == 1 && (tmp[0] == '&' || tmp[0] == '#'))
        {
            send_numeric(client_connected, ERR_BADCHANNAME, "PRIVMSG", "Invalid channel name");
            continue;
        }
        targets.push_back(tmp);
    }
    msg = parse_msg(tokens);
    check_and_send(targets, channels, client_connected, clients, msg);
}
