#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "tcp.cpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: program <machine_name> <port_num>\n";
        return EXIT_FAILURE;
    }
    char * hostname = argv[1];
    char * port = argv[2];

    int socket_fd = create_client(port, hostname);

    int id;
    int num_players;
    recv(socket_fd, &id, sizeof(id), 0);
    recv(socket_fd, &num_players, sizeof(num_players), 0);
    std::cout << "id = " << id;
    std::cout << "\nnum_players = " << num_players;
    std::cout << "\nport = " << port << std::endl;

    //work as a server
    int as_server_fd = create_server("0");
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    if (getsockname(as_server_fd, (struct sockaddr *)&addr, &len) == -1) {
        std::cerr << "Error: getsockname() failed\n";
        exit(EXIT_FAILURE);
    }
    unsigned int port_num = ntohs(addr.sin_port);
    std::cout << "port_num: " << port_num;
    send(socket_fd, &port_num, sizeof(port_num), 0);

    create_server();
    create_client();


    std::cout << "Connected as player " << id << " out of " << num_players << " total players\n";
    int left_id = i - 1;
    if (left_id < 0) {
        left_id = num_players;
    }
    int right_id = i + 1;
    if (right_id == num_players) {
        right_id = 0;
    }

    close(socket_fd);

    return EXIT_SUCCESS;
}