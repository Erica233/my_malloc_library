#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BACK_LOG 100

int create_server(const char * port) {
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
    ((struct sockaddr_in * )(host_info_list->ai_addr))->sin_port = 0;
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
    freeaddrinfo(host_info_list);
    return socket_fd;
}

int create_client(const char * port, const char * hostname) {
    struct addrinfo host_info;
    struct addrinfo * host_info_list;
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: getaddrinfo() failed" << gai_strerror(status);
        std::cerr << hostname << "," << port << std::endl;
        exit(EXIT_FAILURE);
    }
    //socket
    int socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: socket() failed";
        std::cerr << hostname << "," << port << std::endl;
        exit(EXIT_FAILURE);
    }
    //connect
    if (connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1) {
        std::cerr << "Error: connect() failed";
        std::cerr << hostname << "," << port << std::endl;
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
}