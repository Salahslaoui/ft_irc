#include "../includes/channel.hpp"

std::string parse_msg(std::vector<std::string> tokens)
{
    std::string tmp = "";

    if (tokens[2][0] != ':')
        return (tokens[2] + '\n');
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
                    flag = 1;
					std::string broadcast_msg = ":" + client_connected->nickname + "!~" + client_connected->username +
							"@localhost PRIVMSG " + channels[j].name + " :" + msg;
                    channels[j].broadcast(broadcast_msg, *client_connected, true);
                    break;
                }
            }
            if (flag == 0)
            {
                send_numeric(client_connected, ERR_NOSUCHCHANNEL, "PRIVMSG", "This channel doesn't exist\r\n");
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
				std::string broadcast_msg = ":" + client_connected->nickname + "!~" + client_connected->username +
							"@localhost PRIVMSG " + clients[j].nickname + " :" + msg;
                send(clients[j].fd, broadcast_msg.c_str(), broadcast_msg.size(), 0);
                flag = 1;
                break;
            }
        }
        if (flag == 1)
        {
            flag = 0;
            continue;
        }
        send_numeric(client_connected, ERR_NOSUCHNICK, "PRIVMSG", "This client doesn't exist\r\n");
    }
}

void    privmsg(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients)
{
    std::vector<std::string> targets;
    std::string target;
    std::string tmp;
    std::string msg;

    if (tokens.size() < 3)
        return (std::cerr << "Enter the right argument" << std::endl, void());
    if (tokens[1][0] == '#')
    {
        if (tokens[1].size() == 1)
            return (std::cerr << "the channel name is invalid" << std::endl, void());
        
    }
    target = tokens[1];
    std::stringstream ss(target);
    while (std::getline(ss, tmp, ','))
    {
        if (tmp.size() == 1 && (tmp[0] == '&' || tmp[0] == '#'))
        {
            send_numeric(client_connected, ERR_BADCHANNAME, "PRIVMSG", "Invalid channel name\r\n");
            continue;
        }
        targets.push_back(tmp);
    }
    msg = parse_msg(tokens);

    check_and_send(targets, channels, client_connected, clients, msg);
}
