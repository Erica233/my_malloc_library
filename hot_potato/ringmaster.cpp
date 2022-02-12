#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BACK_LOG 100

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: program <port_num> <num_players> <num_hops>\n";
        return EXIT_FAILURE;
    }
    char * port = argv[1];
    int num_players = argv[2];
    int num_hops = argv[3];

    char * hostname = NULL;

    struct addrinfo host_info;
    struct addrinfo * host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: getaddrinfo() failed" << gai_strerror(status);
        std::cerr << hostname << "," << port << std::endl;
        return EXIT_FAILURE;
    }
    //socket
    int socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: socket() failed";
        std::cerr << hostname << "," << port << std::endl;
        return EXIT_FAILURE;
    }
    //bind
    int yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes. sizeof(int)) == -1) {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }
    if (bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1) {
        std::cerr << "Error: bind() failed\n";
        std::cerr << hostname << "," << port << std::endl;
        return EXIT_FAILURE;
    }
    //listen
    if (listen(socket_fd, BACK_LOG) == -1) {
        std::cerr << "Error: listen() failed\n";
        std::cerr << hostname << "," << port << std::endl;
        return EXIT_FAILURE;
    }
    //accept
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connect_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connect_fd == -1) {
        std::cerr << "Error: accept() failed\n";
        return EXIT_FAILURE;
    }
    //read
    char buffer[512];
    recv(client_connect_fd, buffer, 9, 0);
    buffer[9] = 0;
    //write
    std::cout << "Received: " << buffer << std::endl;

    //create potato object
    //start game
    //report results
    //shut down the game
    freeaddrinfo(host_info_list);
    clock(socket_fd);
    return EXIT_SUCCESS;
}