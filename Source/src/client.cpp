#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9001
#define BUFFER_SIZE 1024

void sendCommand(int sockfd, const std::string &cmd) {
    send(sockfd, cmd.c_str(), cmd.size(), 0);
    char buffer[BUFFER_SIZE] = {0};
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    std::cout << buffer;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Connection failed");
        return 1;
    }

    std::cout << "Connected to BLINK DB\n";
    
    sendCommand(sockfd, "*3\r\n$3\r\nSET\r\n$1\r\na\r\n$3\r\n100\r\n");
    sendCommand(sockfd, "*2\r\n$3\r\nGET\r\n$1\r\na\r\n");
    sendCommand(sockfd, "*2\r\n$3\r\nDEL\r\n$1\r\na\r\n");
    sendCommand(sockfd, "*2\r\n$3\r\nGET\r\n$1\r\na\r\n");

    close(sockfd);
    return 0;
}
