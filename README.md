# Binghamton University, School of Computing, Spring 2025

## CS428 Project-3: Web Proxy Server

[This file uses Markdown, so please use correct Markdown syntax when editing the file]: #

### SUMMARY

This project implements a simple proxy server that can forward requests from a client to a server and forward responses from a server to a client. Each time a new client connects, the proxy server opens two new sockets, one for the client and one for the server. A thread handles the forwarding so that multiple clients can be handled simultaneously. The proxy server does not parse or process any request or response that it receives, but it is written to handle chunked HTTP responses.

The webserver implementation is essentially identical to the webserver implemented in the second part of Project 2.

### NOTES, KNOWN BUGS, AND/OR INCOMPLETE PARTS

N/A

### REFERENCES

https://www.w3schools.com/html/html_favicon.asp

https://stackoverflow.com/questions/58783794/having-problems-working-with-sigint-signal-in-c-unix

https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example

https://github.com/Wizardous/TCP-File-Transfer/blob/dbd80ba734c65efdb0f7b19c506ca8a7c640f223/file_server.cpp#L79 

https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c 

### INSTRUCTIONS

[Provide clear and complete step-by-step instructions on how to run and test your project]: #
Compile the webserver with "g++ webserver.cpp -o webserver", and then run it with "./webserver <webserver-port>" to specify port <webserver-port>. Compile the proxy server with "g++ proxyserver.cpp -o proxyserver" and run it with "./proxyserver <proxy-port> <webserver-ip> <webserver-port>". Once the webserver and the proxy server are running, you can make requests from the proxy server's ip address and port to retreive files from the webserver.

### SUBMISSION

I have done this assignment completely on my own. I have not copied it, nor have I given my solution to anyone else. I understand that if I am involved in plagiarism or cheating I will have to sign an official form that I have cheated and that this form will be stored in my official university record. I also understand that I will receive a grade of "0" for the involved assignment and my grade will be reduced by one level (e.g., from "A" to "A-" or from "B+" to "B") for my first offense, and that I will receive a grade of "F" for the course for any additional offense of any kind.

By signing my name below and submitting the project, I confirm the above statement is true and that I have followed the course guidelines and policies.

*Andrew Shen-Costello*
*Senih Okuyucu*

* **Submission date:**
4/24/2025

* **Team member 1 name:**
Andrew Shen-Costello

* **Team member 1 tasks:**
I worked on the proxy-server-to-webserver socket handling as well as the forwarding logic for requests and responses.

* **Team member 2 name (N/A, if not applicable):**
Senih Okuyucu

* **Team member 2 tasks (N/A, if not applicable):**


