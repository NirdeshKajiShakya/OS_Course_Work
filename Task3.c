#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 10
#define MAX_FILES 10
#define XOR_KEY 42

typedef struct {
    char username[50];
    char group[50];
    char hashed_pass[50];
} User;

typedef struct {
    char filename[50];
    char content[256];
    char permissions[10]; // POSIX style: rwxrwxrwx
    int encrypted;
    char owner[50];
    char group[50];
} File;

User users[MAX_USERS];
File files[MAX_FILES];
int user_count = 0;
int file_count = 0;
User current_user;

void audit_log(const char* action, const char* filename) {
    FILE* log = fopen("audit.log", "a"); // Append to persistent log file
    if (log != NULL) {
        fprintf(log, "User '%s' performed '%s' on file '%s'\n", current_user.username, action, filename);
        fclose(log);
    }
}

void xor_cipher(char* str, char* out) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        out[i] = str[i] ^ XOR_KEY; // Symmetric XOR for basic hashing and encryption
    }
    out[len] = '\0';
}

void register_user(char* username, char* password, char* group) {
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].group, group);
    xor_cipher(password, users[user_count].hashed_pass);
    user_count++;
}

int login(char* username, char* password) {
    char hashed[50];
    xor_cipher(password, hashed);
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].hashed_pass, hashed) == 0) {
            strcpy(current_user.username, username);
            strcpy(current_user.group, users[i].group);
            return 1;
        }
    }
    return 0;
}

void create_file(char* name, char* content, int encrypted) {
    File* f = &files[file_count];
    strcpy(f->filename, name);
    strcpy(f->owner, current_user.username);
    strcpy(f->group, current_user.group);
    strcpy(f->permissions, "rwxr-xr--"); // Owner full, group read/execute, others read
    f->encrypted = encrypted;
    
    if (encrypted) {
        xor_cipher(content, f->content); // Encrypt content before storing
    } else {
        strcpy(f->content, content);
    }
    file_count++;
    audit_log("CREATE", name);
}

int check_permission(File* f, char required_perm) {
    int owner_idx = 0, group_idx = 3, others_idx = 6;
    int perm_idx = others_idx; // Default to others
    
    if (strcmp(f->owner, current_user.username) == 0) {
        perm_idx = owner_idx;
    } else if (strcmp(f->group, current_user.group) == 0) {
        perm_idx = group_idx;
    }
    
    for (int i = 0; i < 3; i++) {
        if (f->permissions[perm_idx + i] == required_perm) return 1; // Grant access if specific bit is set
    }
    return 0;
}

void write_file(char* name, char* new_content) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].filename, name) == 0) {
            if (!check_permission(&files[i], 'w')) {
                printf("Permission denied for writing to: %s\n", name);
                return;
            }
            if (files[i].encrypted) {
                xor_cipher(new_content, files[i].content);
            } else {
                strcpy(files[i].content, new_content);
            }
            audit_log("WRITE", name);
            return;
        }
    }
}

void read_file(char* name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].filename, name) == 0) {
            if (!check_permission(&files[i], 'r')) {
                printf("Permission denied for reading: %s\n", name);
                return;
            }
            char buffer[256];
            if (files[i].encrypted) {
                xor_cipher(files[i].content, buffer); // Decrypt in memory before displaying
            } else {
                strcpy(buffer, files[i].content);
            }
            printf("Content of %s: %s\n", name, buffer);
            audit_log("READ", name);
            return;
        }
    }
}

void delete_file(char* name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].filename, name) == 0) {
            if (!check_permission(&files[i], 'w')) { // Write permission required to delete
                printf("Permission denied for deleting: %s\n", name);
                return;
            }
            audit_log("DELETE", name);
            // Shift array elements to fill the gap
            for (int j = i; j < file_count - 1; j++) {
                files[j] = files[j + 1];
            }
            file_count--;
            return;
        }
    }
}

int main() {
    register_user("admin", "password123", "admins");
    login("admin", "password123");
    
    create_file("secret.txt", "Confidential OS Data", 1);
    create_file("readme.txt", "Welcome to the system", 0);
    
    read_file("secret.txt");
    write_file("readme.txt", "Updated system info");
    read_file("readme.txt");
    delete_file("readme.txt");
    
    return 0;
}