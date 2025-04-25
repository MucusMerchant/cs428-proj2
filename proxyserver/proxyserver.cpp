#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <thread>
#include <vector>
#include <signal.h>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include <ctime>

using namespace std;

#define REQUEST_BUFFER_LENGTH 1500
#define RESPONSE_BUFFER_LENGTH 1500
#define FILE_BUFFER_LENGTH 1024

atomic<bool> termFlag{false};
int proxySd = socket(AF_INET, SOCK_STREAM, 0);

void forward_response(int newClientSd, int newServerSd)
{
    char buf[RESPONSE_BUFFER_LENGTH] = {0};
    int bytesRead = recv(newServerSd, (char*)buf, RESPONSE_BUFFER_LENGTH, 0);
    while (bytesRead > 0)
    {
        send(newClientSd, buf, bytesRead, 0);
        bytesRead = recv(newServerSd, (char*)buf, RESPONSE_BUFFER_LENGTH, 0);
    }
}

void worker(int newClientSd, int newServerSd) 
{  
    time_t time;
    char msg[REQUEST_BUFFER_LENGTH] = {0};
    int bytesRead = recv(newClientSd, (char*)msg, REQUEST_BUFFER_LENGTH, 0);
    send(newServerSd, msg, bytesRead, 0);
    time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "proxy-forward," << "server" << "," << this_thread::get_id() << "," << ctime(&time);
    forward_response(newClientSd, newServerSd);
    time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "proxy-forward," << "client" << "," << this_thread::get_id() << "," << ctime(&time);
    close(newClientSd);
    close(newServerSd);
    return;
        
}

void int_handler(int n)
{
    cout << "Waiting for active threads" << endl;
    termFlag.store(true);
    close(proxySd);
    cout << "Bye" << endl;
    exit(0);
    return;
}

int main(int argc, char *argv[])
{
    // ignore sigpipe so we don't terminate whenever a client disconnects unexpectedly
    signal(SIGPIPE, SIG_IGN);
    //need to catch ctrl+c so we can kill all threads (SIGTERM)
    signal(SIGINT, int_handler);
    //for the server, we only need to specify a port number
    int port;
    if(argc != 4)
    {
        cerr << "Usage: <proxy_port> <server_ip> <server_port>" << endl;
        exit(0);
    }
    //grab the port number
    port = atoi(argv[1]);
     
    //setup a socket and connection tools
    sockaddr_in proxyAddr;
    bzero((char*)&proxyAddr, sizeof(proxyAddr));
    proxyAddr.sin_family = AF_INET;
    proxyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    proxyAddr.sin_port = htons(port);

    char *serverIp = argv[2]; port = atoi(argv[3]); 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in serverSockAddr;   
    bzero((char*)&serverSockAddr, sizeof(serverSockAddr)); 
    serverSockAddr.sin_family = AF_INET; 
    serverSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    serverSockAddr.sin_port = htons(port);
    
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    
    if(proxySd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(proxySd, (struct sockaddr*) &proxyAddr, sizeof(proxyAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    vector<thread> threadList;

    while (!termFlag.load())
    {
        listen(proxySd, 5);
        //receive a request from client using accept
        //we need a new address to connect with the client
        sockaddr_in newClientSockAddr;
        socklen_t newClientSockAddrSize = sizeof(newClientSockAddr);
        
        int newClientSd = accept(proxySd, (sockaddr *)&newClientSockAddr, &newClientSockAddrSize);
        if(newClientSd < 0)
        {
            if(termFlag.load())
                break;
            cerr << "Error accepting request from client!" << endl;
            continue;
        }

        int newServerSd = socket(AF_INET, SOCK_STREAM, 0);
        int status = connect(newServerSd, (sockaddr*) &serverSockAddr, sizeof(serverSockAddr));
        if(status < 0)
        {
            cout << "Error connecting to webserver!" << endl;
            continue;
        }
        threadList.push_back(thread(worker, newClientSd, newServerSd));
    }

    for (auto &t : threadList) {
        if (t.joinable()){
            t.join();
        }
    }

    return 0;   
}