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

#endif