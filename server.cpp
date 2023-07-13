// Server code
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <map>
#include <set>

#define PORT 8080

using namespace std;

set<int> clientSockets;
map<int, string> clientNames;

void broadcastMessage(int clientSocket, char* buffer){
    string clientName = clientNames[clientSocket];
    string message = clientName + ": " + buffer;
    for (int socket : clientSockets) {
        if(socket != clientSocket)
            send(socket, message.c_str(), message.size(), 0);
    }
}

void removeUser(int clientSocket){
    if (clientSockets.find(clientSocket) != clientSockets.end()){
        clientSockets.erase(clientSocket);
    }
    clientNames.erase(clientSocket);
    close(clientSocket);
}

void showActiveUsers(string clientName){
    string message = clientName + " disconnected.\n";
    message += "Active Users :\n";
    for (auto client: clientNames){
        message += client.second + "  ";
    }
    for (int socket : clientSockets) {
        send(socket, message.c_str(), message.size(), 0);
    }
}

void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    string clientName = clientNames[clientSocket];
    // Handle client messages
    while (true) {
        // Read client message
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            cerr << clientName <<" disconnected!" << endl;
            break;
        }
        // Broadcast message to others
        broadcastMessage(clientSocket, buffer);
    }
    removeUser(clientSocket);
    showActiveUsers(clientName);
}

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Failed to create socket!" << endl;
        return 1;
    }

    // Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Failed to bind!" << endl;
        return 1;
    }

    // Listen
    if (listen(serverSocket, 3) < 0) {
        cerr << "Failed to listen!" << endl;
        return 1;
    }

    cout << "Server started. Waiting for incoming connections..." << endl;

    vector<thread> clientThreads;

    // Accept incoming connections and handle clients
    while (true) {
        // Accept connection
        newSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (newSocket < 0) {
            cerr << "Failed to accept connection!" << endl;
            return 1;
        }

        char nameBuffer[1024] = {0};
        int nameBytesRead = read(newSocket, nameBuffer, sizeof(nameBuffer));
        if (nameBytesRead > 0) {
            string clientName(nameBuffer);
            clientNames[newSocket] = clientName;
        }
        cout << clientNames[newSocket] <<" connected." << endl;
        clientSockets.insert(newSocket);
        // Create a new thread to handle the client
        thread clientThread(handleClient, newSocket);
        clientThread.detach();  // Detach the thread so it runs independently

        // Store the thread in the vector
        clientThreads.push_back(move(clientThread));
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
