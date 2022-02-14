#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "potato.h"

#define BACK_LOG 100

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: program <port_num> <num_players> <num_hops>\n";
        return EXIT_FAILURE;
    }
    char * port = argv[1];
    int num_players = atoi(argv[2]);
    if (num_players <= 1) {
        std::cerr << "Invalid num_players\n";
        return EXIT_FAILURE;
    }
    int num_hops = atoi(argv[3]);
    if (num_hops < 0 || num_hops > 512) {
        std::cerr << "Invalid num_hops\n";
        return EXIT_FAILURE;
    }
    std::cout << "Potato Ringmaster\n";
    std::cout << "Players = " << num_players << std::endl;
    std::cout << "Hops = " << num_hops << std::endl;

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
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
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

    //build connection with each player
    std::vector<std::string> ips;
    std::vector<int> ports;
    std::vector<int> fds;
    for (int i = 0; i < num_players; i++) {
        //accept
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connect_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (client_connect_fd == -1) {
            std::cerr << "Error: accept() failed\n";
            return EXIT_FAILURE;
        }
        struct sockaddr_in * addr = (struct sockaddr_in *) &socket_addr;
        std::string * ip = inet_ntoa(addr->sin_addr);

        send(client_connect_fd, &i, sizeof(i), 0);
        send(client_connect_fd, &num_players, sizeof(num_players), 0);
        int port;
        recv(client_connect_fd, &port, sizeof(port), 0);
        std::cout << "Player " << i << " is ready to play\n";
        std::cout << "\nnum_players = " << num_players;
        std::cout << "\nip = " << *ip;
        std::cout << "\nport = " << port << std::endl;
        ips.push_back(*ip);
        ports.push_back(port);
        fds.push_back(client_connect_fd);
    }

    //read
    char buffer[512];
    //recv(client_connect_fd, buffer, 9, 0);
    buffer[9] = 0;
    //write
    std::cout << "Received: " << buffer << std::endl;

    //create potato object
    Potato potato(num_hops);
    //start game
    //report results
    //shut down the game
    freeaddrinfo(host_info_list);
    close(socket_fd);
    return EXIT_SUCCESS;
}