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
#define FILE_BUFFER_LENGTH 1024
atomic<bool> termFlag{false};
int serverSd = socket(AF_INET, SOCK_STREAM, 0);


string matchMimeType(string path) 
{
    if (path.size() >= 4) 
    {
        int dotPlace = path.find_last_of(".");
        unordered_map<string, string> contentTypes = 
        {
            {".html", "text/html"},
            {".pdf", "application/pdf"},
            {".css","text/css"},            
            {".jpeg","image/jpeg"},            
            {".jpg","image/jpeg"},   
            {".ico","image/x-icon"},             
        };
        return contentTypes[path.substr(dotPlace)];
    }
    return "text/html";
}

int process_get_request(int socket, char request_buffer[REQUEST_BUFFER_LENGTH], int bytes_read)
{
    fstream file;
    request_buffer[bytes_read] = '\0';
    string request = string(request_buffer);

    if (request.substr(0,4) == "GET ") 
    {
        size_t pos = request.find(" ", 4);
        string path = "." + request.substr(4, pos - 4);
        file.open(path, ios::in | ios::ate);
        int file_size = file.tellg();
        file.close();
        file.open(path, ios::in | ios::binary);
        if(file.is_open())
        {
            //cout<<"Transmitting "<<file_size<<" bytes...\n";
        }
        else
        {
            cout<<"Error loading file " << path << ", Exititng.\n";
            string not_found = "HTTP/1.1 404 File not found\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 123\r\n"
                "\r\n"
                "<!DOCTYPE html>\r\n"
                "<html>\r\n"
                "<head><title>404 Not Found</title></head>\r\n"
                "<body>\r\n"
                "<h1>404 File Not Found</h1>\r\n"
                "</body>\r\n"
                "</html>\r\n";
            send(socket, not_found.c_str(), not_found.size(), 0);
            return 404;
        }
        int bytes_read = 0;
        string contentType = matchMimeType(path);
        string response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: " + contentType + "\r\n" 
            "Content-Length: " + to_string(file_size) + "\r\n"
            "Connection: close\r\n"
            "\r\n";
        send(socket, response.c_str(), response.size(), 0);
        char file_buffer[FILE_BUFFER_LENGTH];
        while (1)
        {
            memset(file_buffer, 0, FILE_BUFFER_LENGTH);
            file.read(file_buffer, FILE_BUFFER_LENGTH);
            if (file.gcount() == 0) break;
            int sent = send(socket, file_buffer, file.gcount(), 0);
            if (sent == -1) break;
        }
        file.close();
        return 200;
    } 
    else 
    {
        string not_found = "HTTP/1.1 400 Bad request\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 122\r\n"
            "\r\n"
            "<!DOCTYPE html>\r\n"
            "<html>\r\n"
            "<head><title>400 Bad Request</title></head>\r\n"
            "<body>\r\n"
            "<h1>400 Bad Request</h1>\r\n"
            "</body>\r\n"
            "</html>\r\n";
        send(socket, not_found.c_str(), not_found.size(), 0);
        return 400;
    }
}

void worker(int newSd) 
{  
    char msg[REQUEST_BUFFER_LENGTH] = {0};
    int bytesRead = recv(newSd, (char*)msg, REQUEST_BUFFER_LENGTH, 0);
    int response_code = process_get_request(newSd, msg, bytesRead);
    close(newSd);
    time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << "server-response," << response_code << "," << this_thread::get_id() << "," << ctime(&time);
    return;
        
}

void int_handler(int n)
{
    cout << "Waiting for active threads" << endl;
    termFlag.store(true);
    close(serverSd);
    cout << "Bye" << endl;
    // exit(0);
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
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    //grab the port number
    port = atoi(argv[1]);
     
    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    vector<thread> threadList;
    cout << "Waiting for a client to connect..." << endl;

    while (!termFlag.load())
    {
        listen(serverSd, 5);
        //receive a request from client using accept
        //we need a new address to connect with the client
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);
        
        int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        if(newSd < 0)
        {
            if(termFlag.load())
                break;
            cerr << "Error accepting request from client!" << endl;
            continue;
        }
        threadList.push_back(thread(worker, newSd));
    }

    for (auto &t : threadList) 
    {
        if (t.joinable())
        {
            t.join();
        }
    }
   
    return 0;   
}