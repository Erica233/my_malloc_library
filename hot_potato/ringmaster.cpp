#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: program <port_num> <num_players> <num_hops>\n";
        return EXIT_FAILURE;
    }

    //socket
    //bind
    //listen
    //accept
    //read
    //write

    //create potato object
    //start game
    //report results
    //shut down the game
    return EXIT_SUCCESS;
}