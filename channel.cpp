#include "includes/channel.hpp"

channel::channel() : max_clients(0), topic_flag(false), i(false), t(false), k(false), l(false)
{}

void channel::remove_client(std::string nickname)
{
    for (std::vector<client_info>::iterator it = clients.begin(); it != clients.end();)
    {
        if (it->nickname == nickname)
            it = clients.erase(it);
        else
            ++it;
    }
}

bool channel::remove_moderator(std::string nickname)
{
    for (std::vector<client_info>::iterator it = moderators.begin(); it != moderators.end();)
    {
        if (it->nickname == nickname)
            it = moderators.erase(it);
        else
            ++it;
    }

    if (moderators.empty() && !clients.empty())
    {
        moderators.push_back(clients.front());

        std::cout << "Promoted " << clients.front().nickname
                  << " to channel operator for " << name << std::endl;
    }

    if (clients.empty())
        return true;
	else
		return false;
}

void channel::remove_invited(std::string nickname)
{
    for (std::vector<client_info>::iterator it = invited.begin(); it != invited.end();)
    {
        if (it->nickname == nickname)
            it = clients.erase(it);
        else
            ++it;
    }
}

void channel::broadcast(const std::string& msg, const client_info& sender, bool flag)
{
    std::string message = msg;
    message += "\r\n";

    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (flag && clients[i].fd == sender.fd)
            continue;


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
