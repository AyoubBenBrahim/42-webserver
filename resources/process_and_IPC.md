A process is defined to be an instance of a program in execution. What this means is that a process is an active entity that is running through the instructions that are specified in the program. Whenever you run the software program that you have written and compiled into an executable file, you create a new process. 

A process corresponds to a single instance of virtual memory as shown in Figure 2.1.3. When you run a program, the process is assigned a new virtual memory instance that includes the code, data, heap, and stack segments to contain run-time information. The process executes the instructions as if it were the only piece of software that existed on that hardware. The fundamental goal of working with processes is to establish isolation of programs. If an error occurs in one process (for instance, if your program triggers a segmentation fault), only that process would be affected; the data in other processes would not be harmed, as they would exist in other instances of virtual memory.

https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/ProcessesOverview.html

how to achive isolation of process

To achieve isolation of processes, modern operating systems use several mechanisms.
 Here are some key techniques employed to establish process isolation:

Virtual Memory:

Process Address Space:

Process Control Block (PCB):

Process Context Switching:

Process Scheduling:

Inter-Process Communication (IPC): Although processes are isolated, there are mechanisms provided by the operating system to allow controlled communication and data sharing between processes. These mechanisms, such as pipes, shared memory, message queues, and sockets, enable processes to exchange data and synchronize their activities while maintaining their isolation.

---

Inter-Process Communication (IPC) refers to the mechanisms and techniques used by processes to exchange data and communicate with each other. IPC allows processes running on the same system, or even on different systems connected over a network, to cooperate, synchronize their activities, and share information. One of the common ways to achieve IPC is through the use of sockets.

Sockets are a fundamental IPC mechanism, particularly in network programming. They provide a communication endpoint that allows processes to send and receive data over a network or within the same system. Sockets provide a reliable, bidirectional, and byte-stream-oriented communication channel between processes.

Here are some key aspects of sockets and their usage in IPC:

1/ Socket Types: Sockets can be classified into two main types: Stream sockets (TCP) and Datagram sockets (UDP). Stream sockets offer reliable, connection-oriented communication, ensuring data arrives in the same order it was sent. Datagram sockets, on the other hand, provide connectionless, unreliable communication, where individual packets (datagrams) may arrive out of order or be lost. The choice between stream and datagram sockets depends on the specific requirements of the IPC scenario.

2/ Socket Addresses: Sockets are identified by unique addresses, typically defined by an IP address and a port number. In the case of TCP/IP networking, the IP address identifies the host, and the port number identifies the specific process or service running on that host. Socket addresses allow processes to locate and establish connections with each other.

3/ Socket API: Programming languages and operating systems provide socket APIs (Application Programming Interfaces) to interact with sockets. These APIs offer functions and methods to create, bind, connect, send, receive, and manage sockets. Common socket APIs include Berkeley Sockets API (BSD Sockets) in Unix-like systems, Winsock API in Windows, and socket libraries in various programming languages like Python, Java, and C++.

4/ Socket Operations: Once a socket is created and configured, processes can use various socket operations to communicate. For example, the bind() operation associates a socket with a specific address, the listen() operation sets the socket to a listening state to accept incoming connections, the connect() operation establishes a connection to a remote socket, and the send() and receive() operations allow processes to send and receive data over the socket.

5/ Socket Protocols: Sockets can use different protocols to facilitate communication. TCP (Transmission Control Protocol) and UDP (User Datagram Protocol) are two common protocols used with sockets. TCP provides reliable, connection-oriented communication, ensuring data integrity and ordered delivery. UDP, on the other hand, offers a lightweight, connectionless, and unreliable communication model, suitable for scenarios where low overhead and simplicity are more important than reliability.

6/ Network Communication: Sockets enable IPC over a network, allowing processes running on different systems to communicate. By establishing socket connections between remote hosts, processes can send data to each other, regardless of their physical location.

Overall, sockets provide a flexible and powerful mechanism for IPC, enabling processes to exchange data, synchronize activities, and cooperate in various scenarios, whether on the same system or across a network. 

























