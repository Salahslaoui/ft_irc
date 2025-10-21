#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#define	USERALREADYJOINED 0
#define USERISJOINED 1
#define NOTINCHANNEL 2
#define BANNEDFROMCHAN 3
#define TOOMANYCHANNELS 4
#define BADCHANNELKEY 5
#define CHANNELISFULL 6
#define NOSUCHCHANNEL 7
#define USERISBANNED 8
#define BADCHANMASK 9
#define USERNOTINCHANNEL -1
#define USERNOTFOUND -1
#define ERR_NEEDMOREPARAMS "461"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_UNKNOWNMODE "472"
#include <iostream>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstdlib>
#include <poll.h>
#include <sstream>
#include <algorithm>

struct client_info
{
    std::string nickname;
    std::string username;
    int fd; // client's socket fd;
    bool invited;
    int has_register;
    int PASS_flag;
    int Nickname_flag;
    int Username_flag;
    
    client_info() : fd(-1), has_register(0), PASS_flag(0), Nickname_flag(0), Username_flag(0) {}
};

class channel
{
    public:
        channel();
        std::string name;
        std::string key;
        std::vector<client_info *> clients;
        std::vector<client_info *> moderators;
        std::vector<client_info *> invited;
        int max_clients;
        std::string topics;
        bool i;
        bool o;
        bool t;
        bool k;
        bool l;
};

void join(std::vector<std::string> tokens, std::vector<channel> &channels, client_info *client_connected);
void mode(std::vector<std::string> tokens, std::vector<channel> &channels, client_info *client_connected);

#endif