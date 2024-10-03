#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 1024 // Adjust this value based on expected number of clients

int main() {
    int server_fd, new_socket, epoll_fd;
    struct sockaddr_in address;
    int opt = 1;
    int n_ready;
    struct epoll_event ev, events[MAX_EVENTS];

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options for reuse
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); // Replace with desired port number

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections with a larger backlog (adjust if needed)
    if (listen(server_fd, SOMAXCONN) < 0) { // Use SOMAXCONN for maximum backlog
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Create an epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080 (adjust if needed)\n");

    // Main loop for handling connections and events
    while (1) {
        n_ready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n_ready == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n_ready; i++) {
            if (events[i].data.fd == server_fd) {
                // New connection request
                new_socket = accept(server_fd, (struct sockaddr*)NULL, NULL);
                if (new_socket == -1) {
                    perror("accept");
                    continue;
                }

                // Set the new socket to non-blocking mode
                if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1) {
                    perror("fcntl");
                    close(new_socket);
                    continue;
                }

                // Add the new socket to the epoll instance for monitoring
                ev.events = EPOLLIN;
                ev.data.fd = new_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
                    perror("epoll_ctl");
                    close(new_socket);
                    continue;
                }

                printf("New connection accepted: %d\n", new_socket);
            } else {
                // Handle data from existing client
                // (Replace this section with your logic for receiving and processing data)
                char buffer[1024];
                int bytes_read = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytes_read == 0) {
                    // Client disconnected
                    printf("Client disconnected: %d\n", events[i].data.fd);
                    close(events[i].data.fd);

                    // Remove the disconnected client from epoll (optional)
                    ev.events = 0;
                    ev.data.fd = events[i].data.fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                } else if (bytes_read == -1) {
                    // Error on receive
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // No data available yet, continue waiting in epoll
                        continue;
                    } else {
                        perror("recv");
                        close(events[i].data.fd);

                        // Remove the disconnected client from epoll (optional)
                        ev.events = 0;
                        ev.data.fd = events[i].data.fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    }
                } else {
                    // Data received from client, process it
                    printf("Received %d bytes from client %d: %s\n", bytes_read, events[i].data.fd, buffer);

                    // (Replace this with your logic for handling received data)
                    // You can send data back to the client using send()

                    // ... Your data processing logic here ...
                }
            }
        }
    }

    return 0;
}