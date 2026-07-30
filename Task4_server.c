#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    free(arg); // Prevent memory leaks for socket descriptors
    
    char buffer[BUFFER_SIZE];
    int authenticated = 0;
    
    read(client_sock, buffer, BUFFER_SIZE);
    
    // Validate credentials against expected protocol string
    if (strncmp(buffer, "AUTH:admin:password123", 22) == 0) {
        authenticated = 1;
        send(client_sock, "AUTH_SUCCESS", 12, 0);
    } else {
        send(client_sock, "AUTH_FAILED", 11, 0);
        close(client_sock);
        return NULL;
    }
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // Clear buffer to prevent residual data artefacts
        int bytes = read(client_sock, buffer, BUFFER_SIZE - 1);
        
        if (bytes <= 0) break; // Client disconnected unexpectedly
        
        if (strncmp(buffer, "QUIT", 4) == 0) break; // Graceful termination request
        
        if (strlen(buffer) > 500) {
            send(client_sock, "ERROR: Payload exceeds 500 chars", 32, 0);
        } else {
            send(client_sock, buffer, bytes, 0); // Echo valid message back
        }
    }
    
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed"); // Robust error handling for network setup
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Allow immediate port reuse after restart
    
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_sock < 0) continue;
        
        pthread_t thread_id;
        int* sock_ptr = malloc(sizeof(int)); // Allocate memory to safely pass socket to thread
        *sock_ptr = client_sock;
        
        pthread_create(&thread_id, NULL, handle_client, sock_ptr);
        pthread_detach(thread_id); // Detach to automatically reclaim resources upon thread exit
    }
    
    close(server_sock);
    return 0;
}