#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <vector>
#include <time.h>

#include "potato.h"

#define BACK_LOG 100
#define MAX_HOST_LEN 255

void print_time() {
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    std::cout << "current time is: " << asctime (timeinfo);
}

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
        exit(EXIT_FAILURE);
    }
    //socket
    int socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: socket() failed";
        std::cerr << hostname << "," << port << std::endl;
        exit(EXIT_FAILURE);
    }
    //bind
    int yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }
    if (strcmp(port, "") == 0) {
        ((struct sockaddr_in *) (host_info_list->ai_addr))->sin_port = 0;
    }
    if (bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1) {
        std::cerr << "Error: bind() failed\n";
        std::cerr << hostname << "," << port << std::endl;
        exit(EXIT_FAILURE);
    }
    //listen
    if (listen(socket_fd, BACK_LOG) == -1) {
        std::cerr << "Error: listen() failed\n";
        std::cerr << hostname << "," << port << std::endl;
        exit(EXIT_FAILURE);
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
        std::cerr << "Error: getaddrinfo() failed - " << gai_strerror(status);
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
        perror("");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
}

void select_read(std::vector<int> & fds, Potato & potato) {
    //std::cout << "start of select_read(): \n";
    //std::cout << "tot_hops: " << potato.tot_hops << std::endl;
    int max_fd = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    for (int i = 0; i < fds.size(); i++) {
        if (fds[i] > max_fd) {
            max_fd = fds[i];
        }
        FD_SET(fds[i], &readfds);
    }
    int rv = select(max_fd + 1, &readfds, NULL, NULL, NULL);
    if (rv == -1) {
        perror("select");
        std::cerr << "Error: select() failed\n";
        exit(EXIT_FAILURE);
    } else if (rv == 0) {
        std::cerr << "Timeout: select()\n";
        exit(EXIT_FAILURE);
    } else {
        //std::cout << "Successful select(): \n";
        for (int i = 0; i < fds.size(); i++) {
            if (FD_ISSET(fds[i], &readfds)) {
                recv(fds[i], &potato, sizeof(potato), MSG_WAITALL);
                //std::cout << "after recv(): \n";
                //std::cout << "tot_hops: " << potato.tot_hops << std::endl;
                //std::cout << "curr_rnd: " << potato.curr_rnd << std::endl;
                break;
            }
        }
    }
}