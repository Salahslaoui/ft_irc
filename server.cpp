#include "includes/channel.hpp"
#include "includes/helper.hpp"

std::string server_password;


std::string numeric_reply_user(const std::string& code)
{
    return ":localhost " + code + " :" + "You may not register" + "\r\n";
}

// helper function to send a numeric reply to a client
void send_numeric_user(client_info* client, const std::string& code)
{
    std::string error = numeric_reply_user(code);
    send(client->fd, error.c_str(), error.size(), 0);
}


// std::string numeric_reply_nick_2(const std::string& code, std::string command, const std::string& message)
// {
//     return ":localhost " + code + " " + command + " :" + message + "\r\n";
// }

// // helper function to send a numeric reply to a client
// void send_numeric_nick_2(client_info* client, const std::string& code, std::string command, const std::string& message)
// {
//     std::string error = numeric_reply_nick(code, command, message);
//     send(client->fd, error.c_str(), error.size(), 0);
// }


void    send_numeric_nick(client_info* client, const std::string& code, std::string command, int flag)
{
    std::string error;
    if (flag == NICK_EMPTY)
        error = ":localhost" + code + " :No nickname given" +  "\r\n";
    else if (flag == NICK_NOT_VALID)
        error = ":localhost" + code + " " + command + " :Erroneus nickname" +  "\r\n";
    else if (flag == NICK_INUSE)
        error = ":localhost" + code + " " + command + " :Nickname is already in use" +  "\r\n";
    send(client->fd, error.c_str(), error.size(), 0);
}

void    accept_client(int server_fd, std::vector<pollfd> &fd, std::vector<client_info> &clients)
{
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
        throw std::runtime_error("accept failed");

    pollfd tmp; bzero(&tmp, sizeof (pollfd));
    tmp.fd = client_fd;
    tmp.events = POLLIN;
    fd.push_back(tmp);

    client_info client_info;
    client_info.fd = client_fd;
    clients.push_back(client_info);

    std::cout << "New client connected to the server !" << std::endl;

}

client_info *find_the_client(int client_fd, std::vector<client_info> &clients)
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].fd == client_fd)
            return &clients[i];
    }
    return(nullptr);
}


bool is_valid_nickname(const std::string &nick)
{

    // Check first character
    char first = nick[0];
    if (!isalpha(first) &&
        first != '[' && first != ']' &&
        first != '\\' && first != '`' &&
        first != '^' && first != '{' &&
        first != '}')
        return false;

    // Check remaining characters
    for (size_t i = 1; i < nick.size(); ++i)
    {
        char c = nick[i];
        if (!isalnum(c) &&
            c != '-' &&
            c != '[' && c != ']' &&
            c != '\\' && c != '`' &&
            c != '^' && c != '{' &&
            c != '}')
            return false;
    }
    return true;
}

void parce_nickname(std::string value, client_info *client)
{
    if (value.find(' ') != std::string::npos)
    {
        // std::string error = "Nickname must be a single word\n";
        // send(client->fd, error.c_str(), error.size(), 0);
        // send_numeric(client, "432", "*", NICK_NOT_VALID);
        send_numeric(client, "432", "*", "Erroneus nickname");
        throw std::runtime_error("error :Nickname must be a single word");
    }

    if (!is_valid_nickname(value))
    {
        // std::string error = "Invalid nickname format\n";
        // send(client->fd, error.c_str(), error.size(), 0);
        send_numeric(client, "432", "*", "Erroneus nickname");
        throw std::runtime_error("error :Erroneus nickname");
    }
}

void    detecte_the_command(std::string request, client_info *client, std::vector<client_info> &clients)
{
    try
    {
        std::istringstream iss(request);
        std::string command;
        std::string value;
        iss >> command;
        std::getline(iss, value);
        value = trim(value);
        if ((!client->PASS_flag || command == "PASS") && (!client->Nickname_flag && !client->Username_flag))
        {
                // std::istringstream iss(request);
                // std::string command;
                // std::string value;
                // iss >> command;
                // std::getline(iss, value);
                // value = trim(value);
                // std::cout << "value : " << value << std::endl;
                if (command != "PASS" && command != "NICK" && command != "USER")
                {
                        send_numeric(client, "421", command, "Unknown command");
                        throw std::runtime_error("error :unknown command");

                }
                if (command == "NICK" || command == "USER")
                {
                    std::string error = "ENTRY THE PASSWORD FIRST!\n";
                    send(client->fd, error.c_str(), error.size(), 0);
                    return;

                }
                if (value != server_password)
                {
                        client->PASS_flag = 0;
                        send_numeric(client, "464", "*", "Password incorrect");
                        throw std::runtime_error("error :Password incorrect");

                }
                        client->PASS_flag = 1;
            }
        else
        {
            std::istringstream iss(request);
            std::string command;
            std::string value;
            iss >> command;
            std::getline(iss, value);
            value = trim(value);

            if (command == "NICK")
            {
                if (value.empty())
                {
                    // std::string error = "Error: nickname cannot be empty!\n";
                    // send(client->fd, error.c_str(), error.size(), 0);
                    send_numeric_nick(client, "431", "", NICK_EMPTY);
                    throw std::runtime_error("error :No nickname given");
                }
                // check if the nickname is already used
                for (size_t i = 0; i < clients.size(); i++)
                {
                    if (clients[i].nickname == value)
                    {
                        // std::string error = "Error: nickname already in use!\n";
                        // send(client->fd, error.c_str(), error.size(), 0);
                        send_numeric(client, "433", "*", "Nickname is already in use");
                        throw std::runtime_error("error :nickname is already in use!");
                    }
                }
                parce_nickname(value, client);
                client->nickname = value;
                client->Nickname_flag = 1;
            }


        else if (command == "USER")
        {
            if (value.empty())
            {
                std::string error = "Error: USER command requires parameters\r\n";
                send(client->fd, error.c_str(), error.size(), 0);
            // send_numeric(client, 461, )
                throw std::runtime_error("error :USER command missing parameters");
            }

            std::istringstream iss(value);
            std::string username, mode, unused, realname;

    // Parse first three fields
            if (!(iss >> username >> mode >> unused))
            {
                send_numeric(client, "461", value, "Not enough parameters");
                // std::string error = "Error: USER command requires at least 4 parameters\r\n";
                // send(client->fd, error.c_str(), error.size(), 0);
                throw std::runtime_error("error :Not enough parameters");
            }

    // Validate mode and unused
            if (mode != "0")
            {
                std::string error = "Error: USER mode must be 0\r\n";
                send(client->fd, error.c_str(), error.size(), 0);
                throw std::runtime_error("error :Invalid USER mode");
            }

            if (unused != "*")
            {
                std::string error = "Error: USER unused parameter must be *\r\n";
                send(client->fd, error.c_str(), error.size(), 0);
                throw std::runtime_error("error :Invalid USER unused parameter");
            }

    // Parse realname (rest of line)
            std::getline(iss, realname);
            while (!realname.empty() && (realname[0] == ' ' || realname[0] == '\t'))
                realname.erase(0, 1);
            if (!realname.empty() && realname[0] == ':')
                realname.erase(0, 1);

    // Store only username
            client->username = username;
            client->Username_flag = 1;

    // std::cout << "Parsed USER => username: " << username
    //           << " | mode: " << mode
    //           << " | unused: " << unused
    //           << " | realname: " << realname << std::endl;
        }


            else
            {
                // std::string error = "Error: command invalid!\n";
                // send(client->fd, error.c_str(), error.size(), 0);
                send_numeric(client, "451", "*", "You have not registered");
                throw std::runtime_error("error :You have not registered");
            }

            if (client->PASS_flag && client->Nickname_flag && client->Username_flag)
			{
				client->has_register = 1;
				std::string nick = client->nickname;
				std::string server_name = "irc.localhost";

				std::string welcome =
					":" + server_name + " 001 " + nick + " :Welcome to the IRC network, " + nick + "!\r\n" +
					":" + server_name + " 002 " + nick + " :Your host is " + server_name + "\r\n" +
					":" + server_name + " 003 " + nick + " :This server was created today\r\n" +
					":" + server_name + " 004 " + nick + " " + server_name + " 1.0 o o\r\n" +
					":" + server_name + " 375 " + nick + " :- " + server_name + " Message of the day - \r\n" +
					":" + server_name + " 372 " + nick + " :- Welcome to this minimal IRC server!\r\n" +
					":" + server_name + " 376 " + nick + " :End of /MOTD command.\r\n";

				send(client->fd, welcome.c_str(), welcome.size(), 0);
				std::cout << "Client registered: " << client->nickname << " (" << client->username << ")" << std::endl;
			}


            // handle the two others command]
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    handle_the_req(client_info *client, std::vector<pollfd> &fds, std::vector<client_info> &clients)
{
    char buffer[2048];
    int bytes_received = recv(client->fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0)
    {
        std::cout << "client disconnected !" << std::endl;
        close (client->fd);
        // remove from fds]
        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].fd == client->fd)
            {
                fds.erase(fds.begin() + i);
                break;
            }
        }
        for (size_t i = 0; i < clients.size(); i++)
        {
            if (clients[i].fd == client->fd)
            {
                clients.erase(clients.begin() + i);
                break;
            }
        }
        return;
    }
    buffer[bytes_received] = '\0';
    client->leftover += buffer; // append new data to any partial previous data

    size_t pos;
    //     // Find either CRLF or LF
    //     size_t pos_crlf = client->leftover.find("\r\n");

    //     // Choose the first valid delimiter found
    //     if (pos_crlf != std::string::npos)
    //         pos = pos_crlf;
    //     else
    //     {
    //         puts("hi");
    //         return; // no complete line yet → wait for next recv()

    //     }
    size_t pos_crlf = client->leftover.find("\r\n");
    size_t pos_lf   = client->leftover.find("\n");

    if (pos_crlf != std::string::npos && (pos_lf == std::string::npos || pos_crlf < pos_lf))
        pos = pos_crlf;
    else if (pos_lf != std::string::npos)
        pos = pos_lf;
    else
        return; // no complete line yet

        // Extract one full IRC message line
        std::string request = client->leftover.substr(0, pos);

       if (pos_crlf != std::string::npos && pos == pos_crlf)
        client->leftover.erase(0, pos + 2); // remove \r\n
    else
        client->leftover.erase(0, pos + 1); // remove \n

        // Enforce IRC max line length (512 chars)
        if (request.length() > 512)
        {
            send_numeric(client, "414", client->nickname, "Input line too long");
            return;
        }
        std::cout << "command is : " << request << std::endl;
        std::cout << "the flag id : " << client->PASS_flag << std::endl;
        // Handle the parsed command
        if (buffer[0] == '\n' && request.empty())
            return;
        detecte_the_command(request, client, clients);
}

void Commands(char *buffer, std::deque<channel> &channels, client_info *client_connected, std::vector<client_info> &clients)
{

    //     
    std::string str(buffer);
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string tmp;

    while (std::getline(ss, tmp, ' '))
	{
		tmp = trim(tmp);      // remove \n, \r, spaces, tabs
		if (!tmp.empty())
			tokens.push_back(tmp);
        else
        {
            if (tokens.empty())
                return (send_numeric(client_connected, ERR_BADCHANNAME, "UKNOWN", "you must enter something\r\n"));
            else
                continue;
        }
	}
	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << tokens[i] << " ";
	std::cout << std::endl;
	if (tokens[0] == "JOIN")
        join(tokens, channels, client_connected);
    else if (tokens[0] == "MODE")
        mode(tokens, channels, client_connected);
    else if (tokens[0] == "PRIVMSG")
        privmsg(tokens, channels, client_connected, clients);
    else if (tokens[0] == "TOPIC")
        topic(tokens, channels, client_connected);
    else if (tokens[0] == "INVITE")
        invite(channels, client_connected, clients, tokens);
    else if (tokens[0] == "KICK")
        kick(tokens, channels, client_connected);
	else if (tokens[0] == "PING")
	{
		if (tokens.size() > 1)
		{
			std::string pong = "PONG " + tokens[1] + "\r\n";
			send(client_connected->fd, pong.c_str(), pong.size(), 0);
		}
	}
    else if (tokens[0] == "USER")
    {
        send_numeric_user(client_connected, "462");
    }
    else if (tokens[0] == "PONG")
    {
    // std::cout << "Received PONG from client " << client_connected->nickname << std::endl;
    return; // no further processing
    }
    else
        send_numeric(client_connected, "421", tokens[0], "Unknown command");
}

void    handle_req(int client_fd ,std::vector<pollfd> &fds, std::vector<client_info> &clients, std::deque<channel> &channels)
{
    client_info *client_connected = find_the_client(client_fd, clients);

    if (client_connected->has_register)
    {
        char buffer[1024];
        int byte_recived = recv(client_connected->fd, buffer, sizeof(buffer) - 1, 0);
        if (byte_recived <= 0)
        {
            std::cout << "client disconnected !" << std::endl;
            close (client_connected->fd);
            // remove from fds
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].fd == client_connected->fd)
                {
                    fds.erase(fds.begin() + i);
                    break;
                }
            }
            for (size_t i = 0; i < clients.size(); i++)
            {
                if (clients[i].fd == client_connected->fd)
                {
                    clients.erase(clients.begin() + i);
                    break;
                }
            }
            return;
        }
        buffer[byte_recived] = '\0';
        client_connected->leftover += buffer; // append new data to any partial previous data


        size_t pos_crlf, pos_lf, pos;


// Process all complete lines
        while (true)
        {
            pos_crlf = client_connected->leftover.find("\r\n");
            pos_lf = client_connected->leftover.find("\n");


            if (pos_crlf != std::string::npos && (pos_lf == std::string::npos || pos_crlf < pos_lf))
                pos = pos_crlf;
            else if (pos_lf != std::string::npos)
                pos = pos_lf;
            else
                break; // no complete line yet


            std::string request = client_connected->leftover.substr(0, pos);


// Remove processed line from leftover
            if (pos == pos_crlf)
                client_connected->leftover.erase(0, pos + 2);
            else
                client_connected->leftover.erase(0, pos + 1);


            if (request.empty())
                continue; // ignore empty lines


            if (request.length() > 512)
            {
                send_numeric(client_connected, "414", client_connected->nickname, "Input line too long");
                continue;
            }


// Copy to clean buffer for Commands
            char clean_buffer[1024];
            std::strncpy(clean_buffer, request.c_str(), sizeof(clean_buffer) - 1);
            clean_buffer[sizeof(clean_buffer) - 1] = '\0';


            Commands(clean_buffer, channels, client_connected, clients);
        }
    }

    else 
        handle_the_req(client_connected, fds, clients); // must handle it }
}

size_t helper_parse(std::string str)
{
    size_t i = 0;
    size_t res = 0;
    while (i <= str.size())
    {
        if (str[i] == ' ' || str[i] == '\t')
            res++;
        i++;
    }
    return res;
}

int   parse_the_input(char *av[])
{
    std::string str = av[1];
    int i = 0;
    while(av[0][i] != '\0')
    {
        if (!isdigit(av[0][i]))
        {
            std::cout << "the port invalid !" << std::endl;
            return(0);
        }
        i++;
    }
    int port = std::atoi(av[0]);
    if (port < 1024 || port > 65535)
    {
        std::cerr << "Invalid port!" << std::endl;
        return 0;
    }
    if (str.empty() || helper_parse(str) == str.size())
    {
        std::cerr << "Invalid password!" << std::endl;
        return(0);
    }
    return(1);
}

void f()
{
	system ("leaks exec 6667 alae");
}

int main(int ac, char *av[])
{
	atexit(f);

    if (ac != 3)
    {
        std::cout << "entre the right argumments!" << std::endl;
        exit(1);
    }

    if (!parse_the_input(&av[1]))
        exit(1);
    server_password = av[2];
    // std::cout << server_password << std::endl;

    std::vector<pollfd> fds;
    std::vector<client_info> clients;
    std::deque<channel> channels;
    pollfd tmp; bzero(&tmp, sizeof (pollfd));

    int server_fd = -1;
    bool server_running = true;
    // char buffer[1024] = {0};
    struct sockaddr_in server_addr;

    try
    {

        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        
        if (server_fd == -1)
            throw std::runtime_error("Socket failed!");
        
        tmp.fd = server_fd;
        tmp.events = POLLIN;
        fds.push_back(tmp);
        int opt = 1;
        
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            throw std::runtime_error("setsokopt failed!");
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(atoi(av[1]));
        
        if (bind(server_fd, (struct  sockaddr*)(&server_addr), sizeof(sockaddr_in)) < 0)
            throw std::runtime_error("bind failed!");

        if (listen(server_fd, SOMAXCONN) < 0)
            throw std::runtime_error("listen failed!");\
	}
    catch (const std::exception& e)
        {
            if (server_fd == -1)
                close(server_fd);
            std::cerr << e.what() << '\n';
            exit(1);
        }
    
    std::cout << "the server start listening on port " << av[1] << ":)" << std::endl;
	signal(SIGPIPE, SIG_IGN);
    while (server_running)
    {
        try
        {
            if (poll(&fds[0], fds.size(), -1)  == -1)
                throw std::runtime_error("poll failed!");
    
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents & POLLIN) {
                    if (fds[i].fd == server_fd) {
                        accept_client(server_fd, fds, clients);  // 90%
                    } else {
                        handle_req(fds[i].fd ,fds, clients, channels);//
                    }
                }
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}


// USER ff 0 * :r