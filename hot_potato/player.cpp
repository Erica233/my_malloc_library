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
    //send(master_fd, )



    close(socket_fd);

    return EXIT_SUCCESS;
}