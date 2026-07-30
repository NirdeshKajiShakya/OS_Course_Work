# Operating Systems & Systems Programming Concepts in C

A collection of C implementations demonstrating core operating systems and low-level systems programming concepts, including thread synchronization, memory page replacement algorithms, file access security, and multi-threaded TCP network programming.

---

## Technical Overview

| Task | Module | Key Concepts & Technologes |
| :--- | :--- | :--- |
| **Task 1** | Concurrent Thread Scheduling | POSIX Threads (`pthread`), Semaphores (`sem_t`), Mutexes, Deadlock Avoidance, Round-Robin Handoff |
| **Task 2** | Page Replacement Simulator | Memory Management, FIFO Algorithm, LRU Algorithm, Page Faults & Hit Ratios |
| **Task 3** | Secure File System & Access Control | POSIX Permission Bitmasks, Symmetric XOR Encryption, Audit Logging, User Authentication |
| **Task 4** | Network Socket Server & Client | TCP Sockets (`AF_INET`), Multi-threaded Server, Protocol Authentication, Buffer Management |

---

## File Breakdown

### Task 1: Thread Synchronization & Scheduling (`Task1.c`)
Simulates a custom Round-Robin CPU scheduler executing worker threads with fixed time quanta ($T_q = 2$).
* **Synchronization:** Utilizes semaphores to strictly control round-robin thread execution order.
* **Deadlock Prevention:** Enforces strict lock ordering (`lock_A` $\rightarrow$ `lock_B`) to prevent circular wait conditions.
* **Race Condition Guard:** Protects shared resources using mutual exclusion locks (`counter_mutex`).

### Task 2: Page Replacement Simulation (`Task2.c`)
Simulates virtual memory page replacement strategies using dynamic memory allocation.
* **FIFO (First-In, First-Out):** Replaces the oldest page loaded into memory via a circular pointer queue.
* **LRU (Least Recently Used):** Tracks access timestamps to evict the least recently referenced page frame.
* **Configurability:** Accepts optional command-line arguments to dynamically configure frame counts.

### Task 3: Secure Virtual File System (`Task3.c`)
A mock file system demonstrating user authentication, dynamic permission validation, and encrypted storage.
* **Access Control:** Evaluates POSIX-style permissions (`rwxr-xr--`) against owner, group, and other users.
* **Encryption:** Uses symmetric XOR bitwise operations for password hashing and file content encryption/decryption.
* **Auditing:** Automatically logs file interactions (`CREATE`, `READ`, `WRITE`, `DELETE`) to a persistent `audit.log` file.

### Task 4: Multi-Threaded TCP Echo Server & Client (`Task4_server.c`, `Task4_client.c`)
A client-server application built on POSIX TCP network sockets.
* **Multi-Threading:** The server spawns detached `pthread` instances per client connection for concurrent request processing.
* **Handshake Protocol:** Requires credential authentication (`AUTH:admin:password123`) prior to echoing client messages.
* **Safety Limits:** Enforces buffer bounds checking, payload size caps (500 chars), and socket port reuse (`SO_REUSEADDR`).

---

## Prerequisites & Compilation

You will need a standard C compiler (such as `gcc`) and standard POSIX system libraries (`pthread`, `netinet`, `sys/socket`).

### GCC Compilation Commands

```bash
# Task 1: Multithreaded Scheduler
gcc Task1.c -o task1 -pthread

# Task 2: Page Replacement
gcc Task2.c -o task2

# Task 3: Virtual File System
gcc Task3.c -o task3

# Task 4: Socket Server and Client
gcc Task4_server.c -o task4_server -pthread
gcc Task4_client.c -o task4_client
```
## Usage & Execution

### Task 1

```bash
./task1
```
### Task 2

Pass the desired frame count as a command-line argument (defaults to 3 if omitted):

```bash
./task2 4
```
### Task 3

```bash
./task3
```

Note: A file named audit.log will be generated in your current directory.

### Task 4

Start the server in one terminal window:

```bash
./task4_server
```
Run the client in a separate terminal window:

```bash
./task4_client
```

Type messages into the client prompt to receive echoes from the server. Type QUIT to disconnect gracefully.
