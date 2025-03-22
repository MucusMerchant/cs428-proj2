# Binghamton University, Spring 2025

## CS428 Project-2: Web Server


### SUMMARY

The program initializes a web server that receives and responds to TCP and HTTP connections, serving files that are available within its directory. The webserver for Part 1 is quite simple. It handles one HTTP request at a time, closing the connection after each request. The webserver for Part 2 supports multithreading and can initialize multiple HTTP and TCP requests at once. New requests are first handled by a welcome socket, which opens a new socket and handles the request in a separate thread. Both servers can handle several file types, including .jpg, .ico, .pdf, and send the appropriate header.

### NOTES, KNOWN BUGS, AND/OR INCOMPLETE PARTS

Program works as described with no incomplete parts. There is also a version of the project identical to the submission version currently running on seniho.com:8050/home.html. 

### REFERENCES

[List any outside resources used]: 
https://www.w3schools.com/html/html_favicon.asp
https://stackoverflow.com/questions/58783794/having-problems-working-with-sigint-signal-in-c-unix
https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example
https://github.com/Wizardous/TCP-File-Transfer/blob/dbd80ba734c65efdb0f7b19c506ca8a7c640f223/file_server.cpp#L79 

### INSTRUCTIONS

[Provide clear and complete step-by-step instructions on how to run and test your project]: #
Compile the project with "g++ webserver2.cpp -o WS2.out", and then run it with "./WS2.out 8050" to specify port 8050
To test the web server, identify the IP address of the machine running the program and visit the web page at "serverIP:8050/home.html" on any browser connected to the same local network. the server will then respond with the proper resources.


### SUBMISSION

I have done this assignment completely on my own. I have not copied it, nor have I given my solution to anyone else. I understand that if I am involved in plagiarism or cheating I will have to sign an official form that I have cheated and that this form will be stored in my official university record. I also understand that I will receive a grade of "0" for the involved assignment and my grade will be reduced by one level (e.g., from "A" to "A-" or from "B+" to "B") for my first offense, and that I will receive a grade of "F" for the course for any additional offense of any kind.

By signing my name below and submitting the project, I confirm the above statement is true and that I have followed the course guidelines and policies.

*Andrew Shen-Costello*
*Senih Okuyucu*

* **Submission date:**
3/21/2025

* **Team member 1 name:**
Andrew Shen-Costello

* **Team member 1 tasks:**
I worked on the HTTP request parsing and response logic that is the basis of webservers for both Part 1 and Part 2. I also worked on the code that creates and manages new threads for each HTTP request in Part 2. Throughout this project, I helped test and debug the webservers with the wget command and the browser.

* **Team member 2 name (N/A, if not applicable):**
Senih Okuyucu

* **Team member 2 tasks (N/A, if not applicable):**
I helped develop dynamic responses to HTTP Requests from the web server through HTTP request parsing, as well as contributions to memory leak testing and multithread development. I also helped with resource location resolution and network troubleshooting. 

