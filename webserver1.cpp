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

using namespace std;

#define REQUEST_BUFFER_LENGTH 1500
#define FILE_BUFFER_LENGTH 1024

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
            cout<<"[LOG] : File is ready to Transmit.\n";
        }
        else
        {
            cout<<"[ERROR] : " << path << " loading failed, Exititng.\n";
            string not_found = "HTTP/1.1 404 File not found\r\n";
            send(socket, not_found.c_str(), not_found.size(), 0);
            return 1;
        }
        int bytes_read = 0;
        string response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n" 
            "Content-Length: " + to_string(file_size) + "\r\n"
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
            cout<<"[thread " << this_thread::get_id() << "] transmitted "<<sent<<" bytes.\n";
        }
        file.close();
    } 
    else 
    {
        string not_found = "HTTP/1.1 400 Bad request\r\n";
        send(socket, not_found.c_str(), not_found.size(), 0);
        return 1;
    }
    return 0;
}

//Server side
int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    //grab the port number
    int port = atoi(argv[1]);
    //buffer to send and receive messages with
    char msg[REQUEST_BUFFER_LENGTH];
     
    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
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
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    
    while(1)
    {
        //accept, create a new socket descriptor to 
        //handle the new connection with client
        int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        if(newSd < 0)
        {
            cerr << "Error accepting request from client!" << endl;
            break;
        }
        cout << "Connected with client!" << endl;
        
        memset(msg, 0, sizeof(msg));//clear the buffer
        int bytesRead = recv(newSd, (char*)&msg, sizeof(msg), 0);
        cout << bytesRead << " " << sizeof(msg) << endl;
        if(!strcmp(msg, "exit"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
        cout << "> " << msg << endl;
        process_get_request(newSd, msg, bytesRead);
        close(newSd);
        cout << "Closed connection" << endl;
    }
    close(serverSd);
    return 0;   
}