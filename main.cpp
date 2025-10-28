#include "includes/server.hpp"


int is_num(std::string num)
{
	if (num.empty())
		return 0;
	for (int i = 0; i < num.size(); i++)
	{
		if (!isdigit(num[i]))
			return 0;
	}
	return 1;
}

int check_pass(std::string str)
{
	bool flag = false;

	if (str.empty())
		return (0);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
			return 0;
	}
	return 1;
}

int parse_checker(int args_size, char **tokens)
{
	if (args_size != 3)
		return std::cerr << "Syntax Error: ./ircserv <port> <password>" << std::endl, 1;
	if (!is_num(tokens[1]))
		return std::cerr << "Syntax Error: Invalid port number!" << std::endl, 1;
	if (!check_pass(tokens[2]))
		return std::cerr << "Syntax Error: Password invalid" << std::endl, 1;	
	return 0;
}

int main(int ac, char **av)
{
	server_info server;
	int fd;

	if (parse_checker(ac, av))
		return 1;
	
	server.set_port(std::atoi(av[1]));
	server.set_password(av[2]);
	
	try
	{
		server.init();
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
} 