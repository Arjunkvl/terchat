#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

std::vector<int> clients;
std::mutex clientMutex;

void brodcast(char (&buffer)[1024], int clientST)
{
     std::lock_guard lock(clientMutex);
     for (int client : clients)
     {
          if (client != clientST)
          {
               send(client, buffer, 1024, 0);
          }
     }
}

void handleClients(int clientSocket)
{
     char buffer[1024] = {0};
     char message[1024] = {0};
     while (true)
     {
          int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
          if (bytesReceived <= 0)
          {
               std::cout << "connection closed." << std::endl;
               break;
          }
          sprintf(message, "%d>", clientSocket);
          strcat(message, buffer);
          brodcast(message, clientSocket);
          memset(message, 0, sizeof(message));
          memset(buffer, 0, sizeof(buffer));
     }
     close(clientSocket);
     sprintf(buffer, "user %d has left.", clientSocket);
     brodcast(buffer, clientSocket);
     std::lock_guard lock(clientMutex);
     auto it = std::remove(clients.begin(), clients.end(), clientSocket);
     clients.erase(it, clients.end());
     std::cout << "client removed" << std::endl;
}
int main()
{
     std::cout <<"<<<<<<Server started>>>>>>"<< std::endl;
     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
     sockaddr_in serverAddress;
     serverAddress.sin_family = AF_INET;
     serverAddress.sin_port = htons(8080);
     serverAddress.sin_addr.s_addr = INADDR_ANY;
     bind(serverSocket, (struct sockaddr *)&serverAddress,
          sizeof(serverAddress));
     int lisStatus = listen(serverSocket, 3);
     if (lisStatus < 0 || serverSocket < 0)
     {
          std::cout << "Error Occured" << std::endl;
          return 1;
     }
     while (true)
     {
          socklen_t addressSize = sizeof(serverAddress);
          int clientSocket = accept(serverSocket, (struct sockaddr *)&serverAddress, &addressSize);
          if (clientSocket < 0)
               continue;
          std::lock_guard lock(clientMutex);
          clients.push_back(clientSocket);
          std::cout << "new user " << clientSocket << " Joined." << std::endl;
          std::thread(handleClients, clientSocket).detach();
     }
     close(serverSocket);
     std::cout << "done.." << std::endl;
     return 0;
}