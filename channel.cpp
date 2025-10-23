#include "includes/channel.hpp"

channel::channel() : i(false), o(false), t(false), k(false), l(false), max_clients(0)
{}

void channel::broadcast(const std::string& msg, const client_info& sender)
{
    std::string message = msg;
    message += "\r\n";

    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i].fd == sender.fd)
            continue; // skip the sender

        ssize_t sent = send(clients[i].fd, message.c_str(), message.size(), 0);
        if (sent == -1)
            perror(("send failed to " + clients[i].nickname).c_str());
        else if ((size_t)sent < message.size())
            std::cerr << "Warning: partial send to " << clients[i].nickname << std::endl;
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
