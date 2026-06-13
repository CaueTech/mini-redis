# Mini-Redis

A robust and simplified Redis-like server implementation written in C. This project supports in-memory key-value storage with collision handling, key expiration (TTL), and concurrent client processing through I/O multiplexing using `select()`.

**Version: Mark II**

## 🚀 Overview

**Mini-Redis** is an educational project focused on gaining a deeper understanding of distributed systems concepts, memory management in C, and high-performance server development from scratch.

In this updated version, the database engine has been extended with data persistence and a Time-To-Live (TTL) manager based on a Min-Heap data structure.

> An educational Redis-inspired implementation featuring a custom Hash Table, Min-Heap-based TTL management, AOF persistence, and an event-driven concurrent TCP server powered by `select()`.

---

## 📊 Technologies and Concepts

* C Programming Language
* TCP/IP Sockets
* I/O Multiplexing with `select()`
* Hash Tables
* Linked Lists
* Min-Heaps
* Append-Only File (AOF) Persistence
* Manual Memory Management
* Event-Driven Architecture

---

## ⚙️ Features

* ✅ **Custom Hash Table:** In-memory storage with collision resolution through linked-list chaining.
* ✅ **I/O Multiplexing:** Handles multiple concurrent clients using `select()` without thread overhead.
* ✅ **TTL (Time-To-Live):** Active and passive key expiration managed through a Min-Heap.
* ✅ **Data Persistence (AOF):** Transaction log recording for database recovery after restarts.
* ✅ **Supported Commands:** `SET`, `GET`, and `DEL`.

---

## 📂 Project Structure

The codebase has been modularized to separate networking, state management, and data structure responsibilities.

```text
.
├── build/                  # Auto-generated object files (.o)
├── headers/                # Header files and definitions
│   ├── server.h
│   └── structs.h
├── main/
│   └── main.c              # Application entry point (main server loop)
├── server/
│   ├── data/               # Data structures and command parsing
│   │   ├── llist_hash.c    # Hash Table and Linked List implementation
│   │   ├── mheap.c         # Min-Heap implementation (TTL management)
│   │   └── parsing.c       # Command parser and validator
│   ├── network.c           # TCP socket and connection management
│   └── states.c            # State machine and command execution
├── log.txt                 # Append-Only persistence file
├── Makefile                # Build automation
└── README.md
```

---

## 🛠️ Build and Run

The project uses a **Makefile** to automate compilation and keep build artifacts organized.

### Prerequisites

* GCC Compiler
* Linux or WSL (Windows Subsystem for Linux)

### I. Build

To compile the project:

```bash
make
```

To remove build artifacts and perform a clean build:

```bash
make clean
make
```

### II. Run

After compilation, start the server:

```bash
./mini_redis
```

The server listens by default on:

```text
localhost:8080
```

To connect using Netcat:

```bash
nc localhost 8080
```

Or using Telnet:

```bash
telnet localhost 8080
```

---

## 💬 Commands and Usage

The database stores key-value pairs.

> **Note:** Commands and key names are **case-sensitive**. Therefore, `SET`, `set`, and `Set` are treated as different commands.

### SET

```text
SET <key> <value> [EX seconds]
```

Creates or updates a key with the specified value.

The optional `EX` modifier sets a TTL (Time-To-Live) in seconds.

**Examples:**

```text
SET user_123 John EX 60
```

Stores the value `John` for 60 seconds.

```text
SET configuration dark_mode
```

Stores the value permanently.

---

### GET

```text
GET <key>
```

Retrieves the value associated with a key.

If the key does not exist or has expired, the server returns an error or null value.

**Example:**

```text
GET user_123
```

---

### DEL

```text
DEL <key>
```

Removes a key immediately from memory.

If the key has an associated TTL, it is also removed from the Min-Heap responsible for expiration tracking.

**Example:**

```text
DEL configuration
```

---

## 💾 Data Persistence (AOF)

To prevent data loss during unexpected shutdowns, the system implements a persistence mechanism inspired by Redis's **Append-Only File (AOF)**.

Whenever a write operation (`SET` or `DEL`) is successfully executed:

1. The operation is applied in memory.
2. The command is immediately appended to `log.txt`.

During server startup, the log file is read sequentially and all commands are replayed internally, rebuilding the Hash Table and Min-Heap state before accepting new client connections.

This approach ensures consistent recovery of the database state after restarts.

---

## 📈 Concepts Explored

This project explores several fundamental Software Engineering and Systems Programming concepts.

### Data Structures

* Hash Tables
* Linked Lists
* Min-Heaps

### Systems Programming

* TCP Sockets
* I/O Multiplexing with `select()`
* Manual Memory Management

### Database Concepts

* Key-Value Storage
* Log-Based Persistence (AOF)
* Key Expiration (TTL)

### Software Architecture

* Component Modularization
* Separation of Concerns
* State Machine-Based Command Processing

---

## 🎯 Project Goals

This project was developed for educational purposes with the objective of deepening knowledge in:

* Distributed Systems Concepts
* High-Performance Server Development
* Low-Level Data Structures
* Data Persistence Mechanisms
* Network Programming in C

Inspired by Redis's architecture, Mini-Redis aims to reproduce its core concepts in a simplified manner, providing a hands-on way to study how modern in-memory databases operate internally.

---

## 📄 License

This project is distributed for educational and learning purposes.