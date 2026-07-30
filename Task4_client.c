#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Client socket creation failed"); // Handle client side socket errors
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }
    
    char buffer[BUFFER_SIZE];
    
    send(sock, "AUTH:admin:password123", 22, 0);
    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);
    
    if (strncmp(buffer, "AUTH_SUCCESS", 12) != 0) {
        printf("Authentication failed.\n");
        close(sock);
        return 1;
    }
    
    printf("Authenticated. Type messages (or QUIT):\n");
    
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove trailing newline character
        
        send(sock, buffer, strlen(buffer), 0);
        
        if (strncmp(buffer, "QUIT", 4) == 0) break;
        
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        printf("Server echo: %s\n", buffer);
    }
    
    close(sock);
    return 0;
}