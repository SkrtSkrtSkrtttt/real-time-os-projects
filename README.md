# Real-Time Operating Systems Projects

A collection of system-level projects built in C/C++ to simulate core OS functionalities including kernel modules, shell creation, process scheduling, and user–kernel communication.

---

## 🧠 Topics Covered

- Kernel module development
- Linked list data structures in C
- Custom Linux shell with redirection, piping, background jobs
- Round-robin process scheduling with I/O interrupts
- User–kernel messaging via Netlink sockets
- Multithreaded producers/subscribers with topic filtering

---

## 📁 Projects

### 🔹 Project 1 – Kernel Module & Linked List
- Wrote a basic kernel module with `printk`
- Implemented insert/delete for a sorted linked list

### 🔹 Project 2 – Custom Linux Shell
- Built a shell using `fork`, `execvp`, `dup2`, `pipe`
- Parsed commands, flags, and handled `|`, `<`, `>`, `&`
- Bonus: Added `cd`, `ls` functionality

### 🔹 Project 3 – Round-Robin Scheduler
- Simulated a 4-core CPU with round-robin logic
- Handled process I/O interrupts and re-queuing

### 🔹 Project 4 – Kernel Pub/Sub System
- Built a Netlink-based pub/sub system
- Kernel tracks dynamic publishers/subscribers via linked lists
- Bonus: Topic-based routing using `rhashtable`
- See:  [Netlink Pub/Sub](https://github.com/SkrtSkrtSkrtttt/netlink-pubsub-rtos)
---

## ⚙️ Tools

- C
- Ubuntu 18.04 via VirtualBox  
---

## 📌 Info

- **Course**: Real-Time Operating Systems  
- **Instructor**: Prof. Daniel Benz  
- **Semester**: Spring 2024  
- **School**: Stony Brook University  
- **Author**: Naafiul Hossain

---

