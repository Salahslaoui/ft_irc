#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client
{
    private:
        std::string nickname;
        std::string username;
        std::string realname;
		std::string f_buffer;
        int registred;
        int pass_auth;
        int user_auth;
        int nick_auth;
        int fd;
		short *poll_check;
    public:
        Client();
        ~Client();
    // getters
        std::string get_nick();
        std::string get_username();
		std::string get_fbuffer();
        std::string get_real_user();
        int get_pass_auth();
        int get_user_auth();
        int get_nick_auth();
        int get_regt();
        int get_fd();
		short *get_revent();
    // setters
        void set_nick(std::string set);
        void set_username(std::string set);
		void set_fbuffer(std::string buffer);
        void set_real_name(std::string set);
        void set_pass_auth(int set);
        void set_user_auth(int set);
        void set_nick_auth(int set);
        void set_regt(int set);
        void set_fd(int set);
		void set_revent(short *nvent);
    // tools

};


#endif