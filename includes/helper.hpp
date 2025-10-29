#ifndef HELPER_HPP
#define HELPER_HPP

#include "channel.hpp"

template<typename T, typename P>
typename T::const_iterator easyfind(const T& container, P to_find)
{
    typename T::const_iterator it = std::find(container.begin(), container.end(), to_find);
    if (it == container.end())
        throw std::runtime_error("couldn't find the element");
    return it;
}

std::string numeric_reply(const std::string& code, const std::string& nick, const std::string& command, const std::string& message);
std::string trim(const std::string& s);
client_info* check_if_op(channel* di_channel, std::string client);
client_info* find_client(const std::string &client_to_find, std::vector<client_info> &clients);
channel* find_channel(const std::string &channel_to_find, std::deque<channel> &channels);
void send_it(Client client, std::string str);
void send_it_cl(client_info *client, std::string str);
std::string get_client_ipp(int client_fd);
client_info converter(Client *client);

#endif