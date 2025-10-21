#include "includes/channel.hpp"

channel::channel() : i(false), o(false), t(false), k(false), l(false), max_clients(0)
{}

void    channel::broadcast(std::string msg, client_info client)
{
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i].fd != client.fd)
            send(clients[i].fd, msg.c_str(), msg.size(), 0);
    }
}

// std::string channel::Modes()
// {
//     std::string tmp = "+";
//     if (i)
//         tmp += "i";
//     if (t)
//         tmp += "t";
//     if (k)
//         tmp += "k";
//     if (l)
//         tmp += "l";
//     return tmp;
// }

std::string channel::get_client_ip(int client_fd)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char ip[INET_ADDRSTRLEN];

    if (getpeername(client_fd, (struct sockaddr*)&addr, &addr_len) == -1)
        return "unknown";

    if (inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN) == NULL)
        return "unknown";

    return std::string(ip);
}
