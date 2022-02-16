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
    std::cout << "port_num: " << port_num << std::endl;
    send(socket_fd, &port_num, sizeof(port_num), 0);

    //create_server();
    //create_client();

    std::cout << "Connected as player " << id << " out of " << num_players << " total players\n";
    int left_id = id - 1;
    if (left_id < 0) {
        left_id = num_players;
    }
    int right_id = id + 1;
    if (right_id == num_players) {
        right_id = 0;
    }
    int right_port;
    char right_host_cstr[255];
    memset(right_host_cstr, 0, sizeof(right_host_cstr));
    recv(socket_fd, &right_port, sizeof(right_port), 0);
    recv(socket_fd, &right_host_cstr, sizeof(right_host_cstr), 0);
    std::string right_host(right_host_cstr);
    std::cout << "sizeof(right_host_cstr): " << sizeof(right_host_cstr) << std::endl;
    std::cout << "sizeof(right_host): " << right_host.size() << std::endl;
    std::cout << "right_id: " << right_id << std::endl;
    std::cout << "right_port: " << right_port << std::endl;
    std::cout << "right_host: " << right_host << std::endl;



    close(socket_fd);
    close(as_server_fd);

    return EXIT_SUCCESS;
}