
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

using namespace std;

void receiveMessages(int clientSocket) {
    char buffer[1024] = {0};
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            cerr << "Server disconnected!" << endl;
            break;
        }
        cout<< buffer << endl;
    }
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Failed to create socket!" << endl;
        return 1;
    }

    // Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0) {
        cerr << "Invalid address or address not supported!" << endl;
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Failed to connect to the server!" << endl;
        return 1;
    }

    cout << "Connected to the server." << endl;
    string clientName;
    cout << "Enter your name: " << endl;
    getline(cin, clientName);
    send(clientSocket, clientName.c_str(), clientName.length(), 0);
    cout << "Welcome " << clientName << endl;
    // Start a thread to receive messages from the server
    thread receiveThread(receiveMessages, clientSocket);

    // Send messages to the server
    string message;
    while (true) {
        getline(cin, message);
        send(clientSocket, message.c_str(), message.length(), 0);
    }

    // Close socket
    close(clientSocket);

    return 0;
}
