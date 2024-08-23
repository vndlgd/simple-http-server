# Simple HTTP Server

## Overview

This project is a simple HTTP server implemented in C, created to deepen my understanding of C programming and HTTP servers. It covers key concepts like socket programming, multi-threading, and basic HTTP protocol handling. This server serves as a learning tool to demonstrate these concepts in action.
![Cat 200 OK Image](ok.jpg)

## Goal

- **Input:** `server.c`.
- **Build and Run:** Compile `server.c` to create the executable.
- **Output:** Serve the `index.html` file on `localhost:8080`, displaying an HTML page with an image of a cat.

## Features

- **Socket Creation:** Establishes a server socket.
- **Binding:** Binds the socket to an IP address and port.
- **Listening:** Listens for incoming connections.
- **Handling Requests:** Accepts client connections and creates threads to handle each request.
- **Serving Files:** Responds with the contents of `index.html` and JPEG images.

## Development Notes

My main challenges were:

- **Serving Different File Types:** It was challenging to realize that serving only index.html would not automatically handle other file types. Handling different types of files, such as images, required additional implementation. 

- **Constructing HTTP Responses:** Crafting the correct HTTP response with appropriate headers and content was a significant challenge due to my lack of experience with file handling in C. Ensuring the responses were correctly formatted for browser recognition was a learning experience. 

To overcome these challenges, I utilized several resources:
- **Beej's Guide to Network Programming:** For foundational knowledge on socket programming and network communication.
- **Tutorials and Guides:** Including [YouTube videos](https://www.youtube.com/watch?v=mStnzIEprH8) and [TutorialsPoint’s HTTP guide](https://www.tutorialspoint.com/http/index.htm) for understanding HTTP protocol basics.

## Final Thoughts

This project was a valuable learning experience in network programming and C. It helped me become more familiar with Makefiles and reinforced my understanding of socket programming. While I used various resources to complete the project, I recognize that there is still much to learn. I plan to continue improving my skills in C.
