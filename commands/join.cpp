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
        return std::cout << "channel created: " << it->first << std::endl, void();
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
                    std::string msg;
                    channels[i].clients.push_back(*client);
                    std::cout << "Clients in the channel: " << channels[i].clients.size() << std::endl;
                    std::cout << channels[i].clients[1].nickname << " a33" << std::endl;
                    std::cout << std::endl;
                    channels[i].broadcast(std::string(":" + client->nickname + "!" + client->username + "@" 
                        + channels[i].get_client_ip(client->fd) + " JOIN " + channels[i].name + "\r\n").c_str(), *client);
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