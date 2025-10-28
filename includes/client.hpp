#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client
{
    private:
        std::string nickname;
        std::string username;
		std::string f_buffer;
        int registred;
        int pass_auth;
        int fd;
    public:
        Client();
        ~Client();
    // getters
        std::string get_nick();
        std::string get_username();
		std::string get_fbuffer();
        int get_pass_auth();
        int get_user_auth();
        int get_nick_auth();
        int get_regt();
        int get_fd();

    // setters
        void set_nick(std::string set);
        void set_username(std::string set);
		void set_fbuffer(std::string buffer);
        void set_pass_auth(int set);
        void set_user_auth(int set);
        void set_nick_auth(int set);
        void set_regt(int set);
        void set_fd(int set);

    // tools

};


#endif