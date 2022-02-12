#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: program <machine_name> <port_num>\n";
        return EXIT_FAILURE;
    }
    char machine_name = argv[1];
    char * port = argv[2];

    struct addrinfo host_info;
    struct addrinfo * host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

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
    //connect
    if (connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1) {
        std::cerr << "Error: connect() failed";
        std::cerr << hostname << "," << port << std::endl;
        return EXIT_FAILURE;
    }
    //write
    char * message = "hi there!";
    send(socket_fd, message, strlen(message), 0);
    //read


    freeaddrinfo(host_info_list);
    close(socket_fd);

    return EXIT_SUCCESS;
}