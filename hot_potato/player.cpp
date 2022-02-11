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
    //socket
    //connect
    //write
    //read
    return EXIT_SUCCESS;
}