#ifndef CHANNEL_HPP
#define CHANNEL_HPP


//    INVITE
#define RPL_INVITING "341"




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
#define RPL_ENDOFNAMES "366"
#define ERR_BADCHANNAME	"479"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERONCHANNEL "443"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_NOSUCHNICK "401"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BADCHANNELKEY "475"	
#define ERR_CHANNELISFULL "471"
#define RPL_NAMREPLY "353"
#define RPL_CHANNELMODEIS "324"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_UNKNOWNMODE "472"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstdlib>
#include <poll.h>
#include <sstream>
#include <algorithm>
#include <limits.h>

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
        std::string get_client_ip(int client_fd);
		void broadcast(const std::string& msg);
        // std::string Modes();
        std::string name;
        std::string key;
        std::vector<client_info> clients;
        std::vector<client_info> moderators;
        std::vector<client_info> invited;
        int max_clients;
        std::string topics;
        bool i;
        bool o;
        bool t;
        bool k;
        bool l;
};

void join(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected);
void invite(std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients, std::vector<std::string> tokens);
channel* find_channel(const std::string &channel_to_find, std::deque<channel> &channels);
client_info* find_client(const std::string &client_to_find, std::vector<client_info> &clients);
client_info* check_if_op(channel* di_channel, std::string client);
void send_numeric(client_info* client, const std::string& code, const std::string& target, const std::string& message);
void mode(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected);
void send_numeric(client_info* client, const std::string& code, const std::string& target, const std::string& message);
void privmsg(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients);
void topic(std::vector<std::string> tokens, std::deque<channel> &channels, client_info *client_connected);

#endif