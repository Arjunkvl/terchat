#include <cstring>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>

std::mutex threadMutex;

void receiveThread(int clientSocket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, 1024);
        recv(clientSocket, buffer, sizeof(buffer), 0);
        {
            std::lock_guard lock(threadMutex);
            std::cout<<"\r\033[K";
            std::cout << buffer<<std::endl;
            std::cout<<">";
            std::cout.flush();
        }
    }
}

int connectToServer(int clientSocket)
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (connect(clientSocket, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) == 0)
    {
        std::cout<<"Started connection."<<std::endl;
        return 1;
    }
    return 0;
}

int sendingThread(int clientSocket)

{
    char buffer[1024];
    while (true)
    {
        std::cout<<">";
        memset(buffer, 0, 1024);
        std::cin.getline(buffer, sizeof(buffer));
        std::cout<<"\033[A\r\033[K";
        std::cout.flush();
        send(clientSocket, buffer, strlen(buffer), 0);
        {
        std::lock_guard lock(threadMutex);
        std::cout<<"you: "<<buffer<<std::endl;
        }
    }
}
int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // connecting to server;
    if (!connectToServer(clientSocket))
    {
        std::cout << "connection Failed..";
        return 0;
    }
    std::thread recThread(receiveThread,clientSocket);
    std::thread sendThread(sendingThread,clientSocket);
    recThread.join();
    sendThread.join();
    close(clientSocket);
    return 0;
}