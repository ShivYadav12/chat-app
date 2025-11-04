#include <iostream>
#include <thread>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

void receiveMessages(int sock) {
    char buffer[1024];
    while (true) {
        ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        std::cout << buffer << std::endl;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    std::thread(receiveMessages, sock).detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") break;
        send(sock, message.c_str(), message.size(), 0);
    }

    close(sock);
    return 0;
}
