# FT_IRC
This is a dual member project for making the ft_irc subject from 42 school

ALLOWED FUNCTIONS:

Everything in C++ 98.
socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept, htons,
htonl, ntohs, ntohl, inet_addr, inet_ntoa, send,
recv, signal, sigaction, lseek, fstat, fcntl, poll
(or equivalent)
___________________________________________________________________
___________________________________________________________________

Basically, this is how we create a server:

Creating a socket: 
The socket() function creates an endpoint for communication and returns a file descriptor that refers to that endpoint.
The file descriptor returned by a successful call will be the lowest-numbered file descriptor not currently open for the process.
This file descriptor is used for further socket operations. 
The domain argument specifies a communication domain; this selects the protocol family which will be used for communication. 
In your case, it's AF_INET for IPv4.

Binding the socket: 
The bind() function assigns a local protocol address to a socket. 
With the Internet protocols, the address is the combination of an IPv4 or IPv6 address and a port number. 
In this case, we're using INADDR_ANY to bind to all available interfaces and a specific port number.

Listening for connections:
The listen() function marks the socket referred to by sockfd as a passive socket, 
that is, a socket that will be used to accept incoming connection requests using accept(). 
The second argument defines the maximum length to which the queue of pending connections for sockfd may grow. 
If a connection request arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED.

Accepting connections:
The accept() function is used to accept a connection on a socket.
It extracts the first connection request on the queue of pending connections for the listening socket,
creates a new connected socket, and returns a new file descriptor referring to that socket. 
The newly created socket is not in the listening state, but can operate read() and write() functions.
That's why we need the server to accept the incoming user before doing anything.
The original socket sockfd is unaffected by this call.

Handling connections: 
Once a connection has been established, both ends can send and receive information. 
We can use the read()/write() functions is used to receive and send data from the client.
However, send() and recv() are more suitable because they work only on socket descriptors,
but let you specify certain options for the actual operation. 
Those functions are slightly more specialized 
(for instance, you can set a flag to ignore SIGPIPE, or to send out-of-band messages...)

Closing the connection: 
When communication with a client is finished, 
the connection needs to be closed to free up system resources that were being used by the connection.
This is typically done with the close() function.
We will close the fd that was given by the accept() function.

select(), poll() and epoll() are functions that manage the connected fd's.
These functions manage the data for each fd that is saved in the struct:

 struct pollfd {
               int   fd;         /* file descriptor */
               short events;     /* requested events */
               short revents;    /* returned events */
           };

For each client, a new struct is created. So we will create an array (or vector in c++) with N positions, 
where N is the max. server members. Here is an example on how we can use poll():


#include <sys/poll.h>

int main() {
  // ... server setup (socket creation, binding, listening)

  struct pollfd fds[MAX_CLIENTS]; // Array to store client info
  int num_fds = 0; // Number of connected clients

  while (1) {
    int ret = poll(fds, num_fds, -1); // Wait indefinitely
    if (ret == -1) {
      // Handle error
    } else if (ret == 0) {
      // Timeout (no events)
      // Iterate through fds array
      for (int i = 0; i < num_fds; i++) {
        if (fds[i].revents & POLLIN) {
          // Data available to read from client
          // ... read data from client
        } else if (fds[i].revents & POLLOUT) {
          // Socket ready for writing
          // ... send data to client
        } else if (fds[i].revents & POLLERR) {
          // Error on socket
          // ... close connection and remove client from fds
        } else if (fds[i].revents & POLLHUP) {
          // Connection closed by client
          // ... close connection and remove client from fds
        } else if (fds[i].fd == listening_socket) {
          // New connection request
          // ... accept connection and add new client to fds
        } else {
          // Timeout on this descriptor
          // ... handle timeout for this client
        }
      }
    } else {
      // At least one event occurred
      // ... handle events as before
    }
  }
}


Note that this is written in pure C and we could use some c++ functions.

_______________________________________________________________________________________________________________________
SPECIFIC DOCUMENTATION:

int sockfd = socket(domain, type, protocol);

socket()  creates  an  endpoint  for communication and returns a file descriptor that refers to that endpoint.
       The file descriptor returned by a successful call will be the lowest-numbered file  descriptor  not  currently
       open for the process.
       
      Common Domains:
    AF_UNIX (Local communication):
        Used for communication between processes on the same machine. 
        It's fast and efficient for local communication but cannot be used for communication across networks. 
        (Refer to unix(7) man page for details)
    AF_INET (IPv4 Internet protocols):
        This is the most commonly used domain for network communication over the internet. 
        It utilizes the IPv4 protocol for routing and addressing packets. 
        (Refer to ip(7) man page)
    AF_INET6 (IPv6 Internet protocols):
        This domain uses the newer IPv6 protocol, 
        which offers a larger address space and improved routing capabilities compared to IPv4. 
        (Refer to ipv6(7) man page)

AF_INET is the default choice for most network applications that require communication over the internet.

int type: The socket has the indicated type, which specifies the communication semantics.
        Choosing the Right Socket Type:
        
    For reliable, ordered, two-way communication: Use SOCK_STREAM.
    For sending independent messages where occasional loss is acceptable: Use SOCK_DGRAM.
    For reliable, sequenced data transfer with packet boundaries: Consider SOCK_SEQPACKET (less common).
    For very low-level network protocol access: Use SOCK_RAW with caution (advanced users only).
    For reliable datagram delivery without order guarantees: Explore SOCK_RDM (specific use cases).

SOCK_STREAM is the default choice for most network applications that require reliable, two-way communication.           

int protocol: When you use AF_INET and SOCK_STREAM in the socket() function, the specific protocol used is TCP (Transmission Control Protocol).

AF_INET: This specifies the communication domain, indicating you'll be using the IPv4 internet protocol family.
SOCK_STREAM: This specifies the socket type, indicating you want a connection-oriented, reliable, two-way byte stream.
Protocol (implicit): Since you're using AF_INET for the internet domain and don't explicitly specify a protocol in the third argument of socket(),
the system typically defaults to TCP (0).

int sockfd = socket(AF_INET, SOCK_STREAM, 0);
___________________________________________________________________

When  a  socket  is  created with socket(2), it exists in a name space (address family) but has no address as‐
       signed to it.  bind() assigns the address specified by addr to the socket referred to by the  file  descriptor
       sockfd.   addrlen  specifies  the size, in bytes, of the address structure pointed to by addr.  Traditionally,
       this operation is called “assigning a name to a socket”.

 It is normally necessary to assign a local address using bind() before a SOCK_STREAM socket may  receive  con‐
       nections (see accept(2)).

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); (returns 0 if succes or -1 if error)

The bind() function in the context you mentioned (referencing the bind(2) man page) is used to associate a local address (IP address and port) with a socket created using the socket() function.
It assigns a unique name (address) to a socket. This name specifies where on the local machine the socket should listen for incoming connections or send outgoing data.
As we can see, it uses a socketfd which is explained above, a pointer to a stuct called addr, and  socklen_t variable.

sockfd: This is the file descriptor of the socket you want to bind an address to. 
The file descriptor is a unique identifier returned by the socket() function when you create a socket.

The  actual  structure passed for the addr argument will depend on the address family.
In network programming, struct sockaddr and struct sockaddr_in are related structures used for specifying network addresses.

           struct sockaddr {
               sa_family_t sa_family;
               char        sa_data[14];
           }

           This is a generic address structure defined in <sys/socket.h>.
           It acts as a base structure for various address families, including IPv4 (TCP/IP) and others. 
           It provides a common interface for network functions to handle different address types.


           struct sockaddr_in {
                unsigned short sin_family;  // Address family (always AF_INET for IPv4)
                unsigned short sin_port;     // Port number (in network byte order)
                struct in_addr sin_addr;     // IP address (32-bit value)
                char sin_zero[8];            // Padding (unused)
          };

          This is a specific address structure defined in <netinet/in.h>. 
          It's used to represent an IPv4 (TCP/IP) address. 
          It inherits from the struct sockaddr base structure and adds additional fields specific to IPv4 addresses.

struct sockaddr is generic and can be used to represent various address families, while struct sockaddr_in is specifically designed for IPv4 addresses.
Fields: struct sockaddr only has the sa_family field to identify the address family. 
struct sockaddr_in inherits this field and adds additional fields like sin_port (port number) and sin_addr (IP address).

Usage: Network functions like bind() and connect() often take a pointer to a struct sockaddr as an argument. 
This allows them to work with different address families by checking the sa_family field and interpreting the remaining data accordingly.
However, when specifically dealing with IPv4 addresses, you would use struct sockaddr_in.

___________________________________________________________________

poll():
La función poll() es una herramienta crucial para la programación de servidores IRC en C/C++. 
Permite monitorizar múltiples entradas y salidas (I/O) de forma simultánea, 
lo que resulta esencial para manejar las conexiones de varios clientes simultáneamente.

___________________________________________________________________

PROTOCOLO INTERNET RELAY CHAT: https://datatracker.ietf.org/doc/html/rfc1459



___________________________________________________________________
___________________________________________________________________

