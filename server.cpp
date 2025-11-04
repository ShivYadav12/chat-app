#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>

std::vector<int> clients;
std::mutex clients_mutex;

void broadcastMessage(const std::string &message, int sender_sock) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client : clients) {
        if (client != sender_sock) {
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

void handleClient(int client_sock) {
    char buffer[1024];
    std::string welcome = "Welcome to the chat!\n";
    send(client_sock, welcome.c_str(), welcome.size(), 0);

    while (true) {
        ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        std::string message = "User" + std::to_string(client_sock) + ": " + buffer;
        broadcastMessage(message, client_sock);
    }

    close(client_sock);
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client_sock), clients.end());
    std::cout << "Client " << client_sock << " disconnected." << std::endl;
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(54000);

    bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    std::cout << "Server started on port 54000..." << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_size);
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_sock);
        std::thread(handleClient, client_sock).detach();
        std::cout << "New client connected: " << client_sock << std::endl;
    }

    close(server_sock);
    return 0;
}
