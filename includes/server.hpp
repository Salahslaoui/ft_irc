#ifndef SERVER_HPP
# define SERVER_HPP

#include "channel.hpp"
#include "client.hpp"
#include <cerrno>

class server_info
{
	private:
		int socket_fd;
		bool server_running;
		std::string server_name;
		std::string server_password;
		int port;
		std::vector<pollfd> pollFds;
		client_info Cl;

		std::vector<Client> clients;
		std::vector<client_info> clients_s;
		std::deque<channel> channels;
	public:
		server_info();

		// getters
		int get_fd();
		int get_port();
		bool is_running();
		std::string name();
		std::string password();
		std::vector<Client> get_clients();

		// setters
		void set_fd(int fd);
		void set_port(int s_port);
		void set_server_run(bool flag);
		void set_name(std::string name);
		void set_password(std::string pass);
		void set_clients(std::vector<Client> set);
		
		void init();
		void run();
		void handle_request(int client_fd);
		Client *get_client(int fd);
		void accept_client();
		void remove_client(int fd);
    	void handle_auth(std::string buffer, Client *client_connected, std::vector<Client> &clients);
};

int 	auth(std::vector<std::string> tokens, Client *client, std::vector<Client> &clients, std::string s_pass, std::deque<channel> &channels);
std::vector<std::string> split(std::string buffer);

#endif