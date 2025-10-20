#include "../channel.hpp"

std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of("\r\n\t ");
    size_t end = s.find_last_not_of("\r\n\t ");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

void    create_channel(std::vector<channel> &channels, std::map<std::string, std::string>::iterator it, server_info *client, std::string &tmp)
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
        add.clients.push_back(client);
        add.moderators.push_back(client);
        channels.push_back(add);
        return std::cout << "channel created: " << it->first << std::endl, void();
    }
    for (int i = 0; i < channels.size(); ++i)
    {
        if ((channels[i].l == 1 && channels[i].clients.size() + 1 <= channels[i].max_clients) || channels[i].l == 0)
        {
            if (it->first == channels[i].name)
            {
                tmp = it->second;
                for (int j = 0; j < channels[i].clients.size(); ++j)
                {
                    if (client->nickname == channels[i].clients[j]->nickname)
                        return std::cerr << "This User already exist in this channel" << std::endl, void();
                }
                if (channels[i].i && client->invited == 0)
                    return (std::cerr << "this channel is invite only the user isn't invited" << std::endl, void());
                else if (channels[i].k && it->second != channels[i].key)
                    return (std::cerr << "the password is incorrect" << std::endl, void());
                else
                {
                    channels[i].clients.push_back(client);
                    return (std::cout << "the client " << client->nickname << " has joined " << channels[i].name << std::endl, void());
                }
            }
        }
        else
            return (std::cerr << "cannot join limit of clients exeeded" << std::endl, void());
    }
    channel add;
    add.name = it->first;
    add.key = it->second;
    add.clients.push_back(client);
    add.moderators.push_back(client);
    channels.push_back(add);
    std::cout << "channel created: " << it->first << std::endl;
}

void join(std::vector<std::string> tokens, std::vector<channel> &channels, server_info *client_connected)
{
    std::string chan;
    std::string ky;
    std::string tmp;
    std::string tmp1;
    std::string tmp2 = "";
    int         err_flag = 0;
    int         alr = 0;
    std::map<std::string, std::string> _channel;
    if (tokens.size() > 3)
        return (std::cerr << "join argument number" << std::endl, void());
    chan = trim(tokens[1]);
    if (tokens.size() == 3)
        ky = trim(tokens[2]);
    else
        ky = "";
    std::stringstream ss(chan);
    std::stringstream ss2(ky);
    while (std::getline(ss, tmp, ','))
    {
        if (tmp[0] != '#' && tmp[0] != '&')
        {
            std::cerr << "the channel first character must be # or &" << std::endl;
            continue;
        }
        if (tmp.size() == 1)
        {
            std::cerr << "the name is invalid" << std::endl;
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
    for (int i = 0; i < channels.size(); ++i)
    {
        std::cout << channels[i].name << ": " << channels[i].key << std::endl;
        std::cout << "Clients in the channel: ";
        for (int j = 0; j < channels[i].clients.size(); ++j)
            std::cout << channels[i].clients[j]->nickname << " ";
        std::cout << std::endl;
        std::cout << "Moderators in the channel" << channels[i].name << " : ";
        for (int j = 0; j < channels[i].moderators.size(); ++j)
            std::cout << channels[i].moderators[j]->nickname << " ";
        std::cout << std::endl;
    }
}