#include "../includes/channel.hpp"

void    privmsg(std::vector<std::string> tokens, std::vector<channel> &channels, client_info *client_connected)
{
    if (tokens.size() < 3)
        return (std::cerr << "Enter the right argument" << std::endl, void());
    if (tokens[1][0] == '#')
    {
        if (tokens[1].size() == 1)
            return (std::cerr << "the channel name is invalid" << std::endl, void());
        
    }
}
