#include "includes/server.hpp"

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

Client server_info::get_client(int fd)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i].get_fd() == fd)
			return (clients[i]);
	}
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

void	server_info::accept_client()
{

	std::cout << "client acccepted 7ashak!" << std::endl;
	sockaddr_in tmp;
	bzero(&tmp, sizeof(sockaddr_in));
	socklen_t size = sizeof(sockaddr_in);
	// fcntl ... ... .
	int client_fd = accept(socket_fd, (sockaddr *)&tmp, &size);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed" << std::endl;
		return;
	}
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl failed" << std::endl;
		return;
	}

	struct pollfd tmp2;

	tmp2.fd = client_fd;
	tmp2.events = POLLIN;
	tmp2.revents = 0;

	pollFds.push_back(tmp2);	
}

std::vector<std::string> split_buffer(std::string buffer)
{
	std::vector<std::string> cmds;
	std::stringstream splitter(buffer);
	std::string line;
	while (std::getline(splitter, line, '\n'))
	{
		line += '\n';
		cmds.push_back(line);
	}
	return (cmds);
}

void	server_info::handle_request(int client_fd)
{
	std::string str;
	std::cout << "request received!" << std::endl;
	char buffer[1024];

	bzero(&buffer, 1024);

	int received = recv(client_fd, buffer, 1024, 0);
	if (received == -1) 
	{
		// remove_client()
		std::cerr << "recv failed!!" << std::endl;
		return ;
	}
	else if (received == 0)
	{
		std::cout << "client disconnected" << std::endl;
		// remove_client()
		return ;
	} 
	buffer[received] = 0;
	str = buffer;
	std::cout << buffer;
	// if (std::string(buffer)[0] == 'a') {
	// 	std::cout << "slawi" << std::endl;
	// }
	if (str.back() == '\n')
	{
		Client C = get_client(client_fd);
		if (C.get_fbuffer().size() + str.size() > 512)
		{
			C.set_fbuffer("");
			std::cerr << "you have exeed the limit" << std::endl;
			return ;
		}
		C.set_fbuffer(C.get_fbuffer() + str);
	}
	else
	{
		Client C = get_client(client_fd);
		if (C.get_fbuffer().size() + str.size() > 512)
		{
			C.set_fbuffer("");
			std::cerr << "you have exeed the limit" << std::endl;
			return ;
		}
		C.set_fbuffer(C.get_fbuffer() + str);
		std::vector<std::string> commands = split_buffer(C.get_fbuffer());
		for (size_t i = 0; i < commands.size(); ++i)
			handle_auth(commands[i], C, clients, server_password);
	}

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
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_fd, (struct sockaddr*)(&server_address), sizeof(sockaddr_in)) < 0)
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
		for (struct pollfd& e : pollFds)
		{
			bool mask = e.revents & POLLIN;
			if (mask) {
				if (e.fd == socket_fd)
				{
					if (e.revents & POLLIN)
						accept_client();
					// if (e.revents & POLLOUT)
					// 	// NOTHING
					// if (false) 
					// 	// remove_client()
				}
				else
				{
					if (e.events & POLLIN) {
						// std::cout << "masked!" << (e.fd & POLLIN) << std::endl;
						handle_request(e.fd);
					}
				}
			}
		}
	}
}