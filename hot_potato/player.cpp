#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "tcp.cpp"
#include "potato.h"

int main(int argc, char **argv) {
    //parse command line argument
    if (argc != 3) {
        std::cerr << "Usage: program <machine_name> <port_num>\n";
        return EXIT_FAILURE;
    }
    char * hostname = argv[1];
    char * master_port = argv[2];

    //connect itself with ringmaster
    int socket_fd = create_client(master_port, hostname);

    int id;
    int num_players;
    recv(socket_fd, &id, sizeof(id), MSG_WAITALL);
    recv(socket_fd, &num_players, sizeof(num_players), MSG_WAITALL);
    //std::cout << "id = " << id << std::endl;
    //std::cout << "\nnum_players = " << num_players;
    //std::cout << "\nmaster_port = " << master_port << std::endl;
    int left_id = id - 1;
    if (left_id < 0) {
        left_id = num_players - 1;
    }
    int right_id = id + 1;
    if (right_id == num_players) {
        right_id = 0;
    }

    char host[MAX_HOST_LEN];
    memset(host, 0, sizeof(host));
    if (gethostname(host, sizeof(host)) != 0) {
        std::cerr << "Error: gethostname() failed\n";
        return EXIT_FAILURE;
    }
    //std::cout << "host from gethostname()" << host << std::endl;
    send(socket_fd, &host, sizeof(host), 0);

    //work as a server and get port, and send to ringmaster
    int as_server_fd = create_server("0");
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    if (getsockname(as_server_fd, (struct sockaddr *)&addr, &len) == -1) {
        std::cerr << "Error: getsockname() failed\n";
        exit(EXIT_FAILURE);
    }
    uint16_t port_num = ntohs(addr.sin_port);
    //std::cout << "my port_num: " << port_num << std::endl;
    send(socket_fd, &port_num, sizeof(port_num), 0);

    uint16_t right_port;
    char right_host_cstr[MAX_HOST_LEN];
    memset(right_host_cstr, 0, sizeof(right_host_cstr));
    recv(socket_fd, &right_host_cstr, sizeof(right_host_cstr), MSG_WAITALL);
    recv(socket_fd, &right_port, sizeof(right_port), MSG_WAITALL);
    std::string right_host(right_host_cstr);
    //std::cout << "right_id: " << right_id << std::endl;
    //std::cout << "right_port: " << right_port << std::endl;
    //std::cout << "right_host: " << right_host << std::endl;
    //std::cout << "right_host_cstr: " << right_host_cstr << std::endl;
    char right_port_cstr[10];
    memset(right_port_cstr, 0, sizeof(right_port_cstr));
    sprintf(right_port_cstr, "%u", right_port);
    //std::cout << "right_port_cstr: " << right_port_cstr << std::endl;

    std::cout << "Connected as player " << id << " out of " << num_players << " total players\n";


    // as a client, connect with right
    //std::cout << "as a client, connect with right: " << std::endl;
    int as_client_fd = create_client(right_port_cstr, right_host_cstr);

    // as a server, connect with left
    //std::cout << "as a server, connect with left: " << std::endl;
    //accept
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connect_fd = accept(as_server_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connect_fd == -1) {
        std::cerr << "Error: accept() failed\n";
        return EXIT_FAILURE;
    }

    //play
    //std::cout << "before potato: \n";
    Potato potato;
    std::vector<int> fds;
    std::vector<int> ids;
    fds.insert(fds.end(), {as_client_fd, client_connect_fd, socket_fd});
    ids.insert(ids.end(), {right_id, left_id});
    srand((unsigned int)time(NULL) + id);
    while (true) {
        std::cout << "------enter while\n";
        print_time();
        //receive potato from ringmaster or other players
        select_read(fds, potato);
        std::cout << "potato.remain_hops: " << potato.remain_hops << std::endl;
        std::cout << "after select_read()\n";
        print_time();
        //std::cout << "tot_hops: " << potato.tot_hops << std::endl;
        //std::cout << "curr_rnd: " << potato.curr_rnd << std::endl;
        //if the ringmaster notify that the game ends, jump out of loop
        if (potato.remain_hops == -1) {
            std::cout << "game ends\n";
            break;
        }
        //if get potato from other player, edit potato
        //std::cout << "edit potato: \n";
        std::cout << "curr_rnd: " << potato.curr_rnd << std::endl;
        potato.ids[potato.curr_rnd] = id;
        std::cout << "ids[potato.curr_rnd] = " << potato.ids[potato.curr_rnd] << std::endl;
        potato.curr_rnd++;
        //std::cout << "curr_rnd: " << potato.curr_rnd << std::endl;
        potato.remain_hops--;
        std::cout << "potato.remain_hops: " << potato.remain_hops << std::endl;
        if (potato.remain_hops == 0) {
            std::cout << "I’m it\n";
            //send to ringmaster
            potato.remain_hops = -1;
            send(socket_fd, &potato, sizeof(potato), 0);
            continue;
        }
        //send to a random neighbor
        int random_idx = rand() % 2;
        send(fds[random_idx], &potato, sizeof(potato), 0);
        //std::cout << "leftid & rightid: " << ids[1] << " " << ids[0] << "\n";
        std::cout << "Sending potato to " << ids[random_idx] << std::endl;
    }

    close(socket_fd);
    close(as_server_fd);
    close(as_client_fd);
    close(client_connect_fd);

    return EXIT_SUCCESS;
}