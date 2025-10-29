#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error: cannot create socket\n";
        return 1;
    }

    // Server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234); // example port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cerr << "Error: cannot connect to server\n";
        close(sockfd);
        return 1;
    }

    std::cout << "Connected to server successfully.\n";

    // Send something to the server
    const char *msg = "Hello server!\r\n";
    send(sockfd, msg, strlen(msg), 0);

    // std::cout << "Message sent. Simulating ungraceful disconnect...\n";


    return 0;
}
