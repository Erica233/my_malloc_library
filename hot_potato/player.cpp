#include <iostream>
#include <cstring>
#include <string>
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
    char * master_port = argv[2];

    int socket_fd = create_client(master_port, hostname);

    int id;
    int num_players;
    recv(socket_fd, &id, sizeof(id), 0);
    recv(socket_fd, &num_players, sizeof(num_players), 0);
    std::cout << "id = " << id;
    std::cout << "\nnum_players = " << num_players;
    std::cout << "\nmaster_port = " << master_port << std::endl;

    //work as a server
    int as_server_fd = create_server("0");
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    if (getsockname(as_server_fd, (struct sockaddr *)&addr, &len) == -1) {
        std::cerr << "Error: getsockname() failed\n";
        exit(EXIT_FAILURE);
    }
    uint16_t port_num = ntohs(addr.sin_port);
    std::cout << "port_num: " << port_num << std::endl;

    //send(socket_fd, &port_num, sizeof(port_num), 0);
    send(socket_fd, &port_num, sizeof(port_num), 0);

    std::cout << "Connected as player " << id << " out of " << num_players << " total players\n";
    int left_id = id - 1;
    if (left_id < 0) {
        left_id = num_players;
    }
    int right_id = id + 1;
    if (right_id == num_players) {
        right_id = 0;
    }
    uint16_t right_port;
    char right_host_cstr[255];
    memset(right_host_cstr, 0, sizeof(right_host_cstr));
    recv(socket_fd, &right_port, sizeof(right_port), 0);
    recv(socket_fd, &right_host_cstr, sizeof(right_host_cstr), 0);
    std::string right_host(right_host_cstr);
    std::cout << "right_id: " << right_id << std::endl;
    std::cout << "right_port: " << right_port << std::endl;
    std::cout << "right_host: " << right_host << std::endl;
    std::cout << "right_host_cstr: " << right_host_cstr << std::endl;
    char right_port_cstr[10];
    memset(right_port_cstr, 0, sizeof(right_port_cstr));
    sprintf(right_port_cstr, "%u", right_port);
    std::cout << "right_port_cstr: " << right_port_cstr << std::endl;
    // as a client, connect with right
    int as_client_fd = create_client(right_port_cstr, right_host_cstr);

    // as a server, connect with left
    //accept
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connect_fd = accept(as_server_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connect_fd == -1) {
        std::cerr << "Error: accept() failed\n";
        return EXIT_FAILURE;
    }

    char buf_send[50];
    memset(buf_send, 0, 50);
    sprintf(buf_send, "my id is %d, my right id is %d\n", id, right_id);
    std::cout << "buf_send: " << buf_send << std::endl;
    char buf_rec[50];
    memset(buf_rec, 0, 50);
    send(as_client_fd, &buf_send, sizeof(buf_send), 0);
    recv(client_connect_fd, &buf_rec, sizeof(buf_rec), 0);
    std::cout << "buf_rec: " << buf_rec << std::endl;

    close(socket_fd);
    close(as_server_fd);
    close(as_client_fd);
    close(client_connect_fd);

    return EXIT_SUCCESS;
}