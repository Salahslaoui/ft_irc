#ifndef SERVER_HPP
# define SERVER_HPP

#include "channel.hpp"

class server_info
{
	private:
		int socket_fd;
		bool server_running;
		std::string server_name;
		std::string server_password;
		int port;
		std::vector<pollfd> pollFds;

		// std::vector<> clients;
		// std::vector<> channels;
	public:
		server_info();

		// getters
		int get_fd();
		int get_port();
		bool is_running();
		std::string name();
		std::string password();

		// setters
		void set_fd(int fd);
		void set_port(int s_port);
		void set_server_run(bool flag);
		void set_name(std::string name);
		void set_password(std::string pass);
		
		void init();
		void run();
		void handle_request(int client_fd);
		void accept_client();
};

#endif