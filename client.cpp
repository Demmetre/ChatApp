// Client code
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void receiveMessages(int clientSocket) {
    char buffer[1024] = {0};
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            std::cerr << "Server disconnected!" << std::endl;
            break;
        }
        std::cout<< buffer << std::endl;
    }
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket!" << std::endl;
        return 1;
    }

    // Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0) {
        std::cerr << "Invalid address or address not supported!" << std::endl;
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to the server!" << std::endl;
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;
    std::string clientName;
    std::cout << "Enter your name: ";
    std::getline(std::cin, clientName);
    send(clientSocket, clientName.c_str(), clientName.length(), 0);

    // Start a thread to receive messages from the server
    std::thread receiveThread(receiveMessages, clientSocket);

    // Send messages to the server
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        send(clientSocket, message.c_str(), message.length(), 0);
    }

    // Close socket
    close(clientSocket);

    return 0;
}
