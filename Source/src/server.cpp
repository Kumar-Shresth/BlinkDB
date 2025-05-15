#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include <unordered_map>
#include "storageEngine.h"

#define PORT 9001
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

StorageEngine db; // Our database instance

// Function to parse RESP-2 command
std::vector<std::string> parseRESP(const std::string &message) {
    std::vector<std::string> tokens;
    size_t pos = 0, start = 0;
    while ((pos = message.find("\r\n", start)) != std::string::npos) {
        std::string part = message.substr(start, pos - start);
        if (!part.empty() && part[0] != '*' && part[0] != '$') { // Ignore RESP length markers
            tokens.push_back(part);
        }
        start = pos + 2;
    }
    return tokens;
}

// Function to process client request
std::string handleCommand(const std::string &request) {
    auto tokens = parseRESP(request);
    if (tokens.empty()) return "-Error\r\n";

    std::string command = tokens[0];
    if (command == "SET" && tokens.size() == 3) {
        db.set(tokens[1], tokens[2]);
        return "+OK\r\n";
    } else if (command == "GET" && tokens.size() == 2) {
        std::string value = db.get(tokens[1]);
        return (value == "NULL") ? "$-1\r\n" : "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    } else if (command == "DEL" && tokens.size() == 2) {
        db.del(tokens[1]);
        return ":1\r\n";
    }
    return "-Invalid Command\r\n";
}

int main() {
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct epoll_event ev, events[MAX_EVENTS];

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server details
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    // Add server socket to epoll
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_fds; i++) {
            if (events[i].data.fd == server_fd) {
                // Accept new client
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("Accept failed");
                    continue;
                }
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                std::cout << "Client connected" << std::endl;
            } else {
                // Handle client request
                char buffer[BUFFER_SIZE] = {0};
                int bytes_read = read(events[i].data.fd, buffer, BUFFER_SIZE);
                if (bytes_read <= 0) {
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    std::cout << "Client disconnected" << std::endl;
                } else {
                    std::string response = handleCommand(std::string(buffer, bytes_read));
                    send(events[i].data.fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
