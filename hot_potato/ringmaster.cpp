#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "potato.h"
#include "tcp.cpp"

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

    int socket_fd = create_server(port);

    //connect ringmaster and each player
    std::vector<std::string> hosts;
    std::vector<uint16_t> ports;
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

        char host_cstr[255];
        memset(host_cstr, 0, sizeof(host_cstr));
        if (inet_ntop(AF_INET, &(((struct sockaddr_in *)&socket_addr)->sin_addr), host_cstr, INET_ADDRSTRLEN) == NULL) {
            std::cerr << "Error: inet_ntop() failed\n";
            return EXIT_FAILURE;
        }
        std::cout << "host_cstr: " << host_cstr << std::endl;
        std::string host(host_cstr);
        std::cout << "host: " << host << std::endl;
        hosts.push_back(host);

        send(client_connect_fd, &i, sizeof(i), 0);
        send(client_connect_fd, &num_players, sizeof(num_players), 0);

        uint16_t port;
        recv(client_connect_fd, &port, sizeof(port), 0);
        std::cout << "Player " << i << " is ready to play\n";
        std::cout << "num_players = " << num_players;
        std::cout << "\nport = " << port << std::endl;
        ports.push_back(port);
        fds.push_back(client_connect_fd);
        std::cout << std::endl;
    }

    //ring together players
    for (int i = 0; i < num_players; i++) {
        int right_id = i + 1;
        if (right_id == num_players) {
            right_id = 0;
        }
        std::cout << "right_id: " << right_id << std::endl;
        std::cout << "right_port (ports[right_id]): " << ports[right_id] << std::endl;
        std::cout << "right_host (hosts[right_id]): " << hosts[right_id] << std::endl;
        send(fds[i], &ports[right_id], sizeof(ports[right_id]), 0);
        send(fds[i], hosts[right_id].data(), hosts[right_id].size(), 0);
        std::cout << std::endl;
    }

    //create potato object
    Potato potato();
    //start game
    std::cout << "Ready to start the game, sending potato to player 2" << std::endl;
    send(fds[0], &potato, sizeof(potato), 0);
    //report results
    //shut down the game

    close(socket_fd);
    return EXIT_SUCCESS;
}