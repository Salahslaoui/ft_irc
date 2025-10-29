#include "includes/server.hpp"
#include "includes/helper.hpp"

server_info::server_info() : socket_fd(0), server_running(false)
{}

int server_info::get_fd()
{
	return socket_fd;
}

int server_info::get_port()
{
	return port;
}

bool server_info::is_running()
{
	return server_running;
}

std::vector<Client> server_info::get_clients()
{
	return (clients);
}

std::string server_info::name()
{
	return server_name;
}

Client *server_info::get_client(int fd)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i].get_fd() == fd)
			return (&clients[i]);
	}

	return (NULL);
}

std::string server_info::password()
{
	return server_password;
}

void server_info::set_fd(int fd)
{
	socket_fd = fd;
}

void server_info::set_port(int s_port)
{
	port = s_port;
}

void server_info::set_clients(std::vector<Client> set)
{
	clients = set;
}

void server_info::set_server_run(bool flag)
{
	server_running = flag;
}

void server_info::set_name(std::string name)
{
	server_name = name;
}

void server_info::set_password(std::string pass)
{
	server_password = pass;
}



void server_info::remove_client(int fd)
{
	client_info *client_connected = NULL;
	for (size_t i = 0; i < clients_s.size(); i++)
	{
		if (clients_s[i].fd == fd)
			client_connected = &clients_s[i];
	}
	if (client_connected)
	{	
			std::string quitMsg = ":" + client_connected->nickname + "!" + client_connected->username + 
							  "@" + get_client_ipp(client_connected->fd) + " QUIT :Client disconnected\r\n";
			for (size_t i = 0; i < channels.size(); ++i)
			{
				for (size_t j = 0; j < channels[i].clients.size();)
				{
					if (channels[i].clients[j].nickname == client_connected->nickname)
					{
						channels[i].clients.erase(channels[i].clients.begin() + j, channels[i].clients.begin() + j + 1);
						channels[i].broadcast(quitMsg, *client_connected, 1);
					}
					else
						j++;
				}
				for (size_t j = 0; j < channels[i].moderators.size();)
				{
					if (channels[i].moderators[j].nickname == client_connected->nickname)
					{
						if (channels[i].moderators.size() == 1 && channels[i].clients.size() >= 1)
						{
							std::string modeMsg = ":ircserver MODE " + channels[i].name + " +o " + channels[i].clients[0].nickname + "\r\n";
							channels[i].moderators.erase(channels[i].moderators.begin() + j, channels[i].moderators.begin() + j + 1);
							channels[i].moderators.push_back(channels[i].clients[0]);
							channels[i].broadcast(modeMsg, *client_connected, 1);
						}
						else if (channels[i].moderators.size() == 1 && channels[i].clients.size() == 0)
						{
							channels.erase(channels.begin() + i, channels.begin() + i + 1);
							++i;
							break;
						}
						else
							j++;
					}
				}
			}
	}

    close(fd);
    for (size_t i = 0; i < clients.size(); ++i)
        if (clients[i].get_fd() == fd)
            clients.erase(clients.begin() + i);
    // for (size_t i = 0; i < clients_s.size(); ++i)
    //     if (clients_s[i].fd == fd)
    //         clients_s.erase(clients_s.begin() + i);
    for (size_t i = 0; i < pollFds.size(); ++i)
        if (pollFds[i].fd == fd)
            pollFds.erase(pollFds.begin() + i);
}


void	server_info::accept_client()
{
	std::cout << "client acccepted 7ashak!" << std::endl;
	Client inst;
	struct sockaddr_in tmp;
	bzero(&tmp, sizeof(sockaddr_in));
	socklen_t size = sizeof(tmp);
	int client_fd = accept(socket_fd, (sockaddr *)&tmp, &size);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed" << std::endl;
		return;
	}
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl failed" << std::endl;
		close (client_fd);
		return;
	}

	struct pollfd tmp2;

	tmp2.fd = client_fd;
	tmp2.events = POLLIN ;
	tmp2.revents = 0;

	pollFds.push_back(tmp2);

	inst.set_fd(client_fd);
	inst.set_revent(&tmp2.revents);
	clients.push_back(inst);
}

std::vector<std::string> split_buffer(std::string buffer)
{
	std::vector<std::string> cmds;
	std::stringstream splitter(buffer);
	std::string line;
	while (std::getline(splitter, line, '\n'))
		cmds.push_back(line);
	return (cmds);
}

int	server_info::handle_request(int client_fd)
{
	std::string str;
	// std::cout << "request received!" << std::endl;
	char buffer[1024];

	bzero(&buffer, 1024);

	int received = recv(client_fd, buffer, 1024, 0);
	if (received == -1) 
	{
		std::cerr << "recv failed!!" << std::endl;
		remove_client(client_fd);
		return -1;
	}
	else if (received == 0)
	{
		std::cout << "client disconnected" << std::endl;
		remove_client(client_fd);
		return  -1;
	} 
	buffer[received] = 0;
	str = buffer;
	std::cout << buffer;

		Client *C = get_client(client_fd);
		if (C->get_fbuffer().size() + str.size() > 512)
		{
			C->set_fbuffer("");
			std::cerr << "you have exeed the limit" << std::endl;
			return 0;
		}
		C->set_fbuffer(C->get_fbuffer() + str);
		std::vector<std::string> commands = split_buffer(C->get_fbuffer());
		for (size_t i = 0; i < commands.size(); ++i)
		{
			if (trim(commands[i]).empty())
        		continue;
			handle_auth(commands[i], C, clients);
		}
		C->set_fbuffer("");
	return 0;
}

void server_info::init()
{
	int opt = 1;

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		throw std::runtime_error("Error: socket failed!");

	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close (socket_fd);
		throw std::runtime_error("Error: setsockopt failed!");
	}

	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl failed!");

	struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

	if (bind(socket_fd, (struct sockaddr*)(&server_address), sizeof(server_address)) < 0)
	{
		close(socket_fd);
		throw std::runtime_error("Error: can't bind to port");
	}
	if (listen(socket_fd, SOMAXCONN) < 0)
	{
		close (socket_fd);
		throw std::runtime_error("Error: failed to listen on socket!");
	}

	struct pollfd s_poll;
    s_poll.fd = socket_fd;
    s_poll.events = POLLIN | POLLOUT | POLLERR;
    s_poll.revents = 0;
    pollFds.push_back(s_poll);

	server_running = true;
	std::cout << "the IRC server is now listening on port " << port << std::endl;
}

void server_info::run()
{
	std::cout << "IRC server is running!" << std::endl;
	while (server_running)
	{
		if (poll(&pollFds[0], pollFds.size(), -1) == -1)
			throw std::runtime_error("poll faiiiiiiiiiiiled!");
		for (size_t i = 0; i < pollFds.size() ; i++)
		{
			if (pollFds[i].revents & POLLIN)
			{
				if (pollFds[i].fd == socket_fd)
				{
					if (pollFds[i].revents & POLLIN)
						accept_client();
				}
				else
				{
					if (pollFds[i].events & POLLIN  && !(pollFds[i].revents & POLLERR))
					{
						if (handle_request(pollFds[i].fd))
							i--;
					}
				}
			}
			if (pollFds[i].revents & POLLERR) 
			{
				if (pollFds[i].fd != socket_fd)
				{
					remove_client(pollFds[i].fd);
					i--;
				}
			}
		}
	}
}
