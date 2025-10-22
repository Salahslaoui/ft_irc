#include "../includes/helper.hpp"

std::string trim(const std::string& s)
{
	if (s.empty()) return "";
    size_t start = s.find_first_not_of("\r\n\t ");
    size_t end = s.find_last_not_of("\r\n\t ");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

std::string numeric_reply(const std::string& code, const std::string& nick, const std::string& command, const std::string& message)
{
    return ":localhost " + code + " " + nick + " " + command + " :" + message;
}

// helper function to send a numeric reply to a client
void send_numeric(client_info* client, const std::string& code,
                  const std::string& target, const std::string& message)
{
    std::string error = numeric_reply(code, client->nickname, target, message);
    send(client->fd, error.c_str(), error.size(), 0);
}
