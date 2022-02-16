#include <iostream>
#include <cstring>
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
        std::cout << "inet_ntoa: " << inet_ntoa(((struct sockaddr_in *)&socket_addr)->sin_addr) << std::endl;
        //std::string host;
        char host[255];
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&socket_addr)->sin_addr), host, INET_ADDRSTRLEN);
        std::cout << "host: " << host << std::endl;
        //ips.push_back(host);

        send(client_connect_fd, &i, sizeof(i), 0);
        send(client_connect_fd, &num_players, sizeof(num_players), 0);

        int port;
        recv(client_connect_fd, &port, sizeof(port), 0);
        std::cout << "Player " << i << " is ready to play\n";
        std::cout << "num_players = " << num_players;
        std::cout << "\nport = " << port << std::endl;
        ports.push_back(port);
        fds.push_back(client_connect_fd);
    }

    //ring together players
    for (int i = 0; i < num_players; i++) {
        int right_id = i + 1;
        if (right_id == num_players) {
            right_id = 0;
        }
        int right_port;
        std::cout << "right_id: " << right_id << std::endl;
        std::cout << "right_port (ports[right_id]): " << ports[right_id] << std::endl;
        send(fds[i], &ports[right_id], sizeof(ports[right_id]), 0);
        //send(fds[i], &)

    }



    //create potato object
    Potato potato(num_hops);
    //start game
    //report results
    //shut down the game

    close(socket_fd);
    return EXIT_SUCCESS;
}