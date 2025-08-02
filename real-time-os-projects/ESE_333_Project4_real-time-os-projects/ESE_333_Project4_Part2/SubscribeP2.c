//Naafiul Hossain

//Part 2 subscribe 

#include <linux/netlink.h>  // Netlink library for communication with the kernel
#include <stdio.h>          // Standard input/output functions
#include <stdlib.h>         // Standard library functions
#include <string.h>         // String manipulation functions
#include <sys/socket.h>     // Socket-related functions and structures
#include <unistd.h>         // Standard symbolic constants and types
#include <pthread.h>        // POSIX threads library

#define NETLINK_USER 31     // Custom netlink protocol identifier

#define MAX_PAYLOAD 1024    // Maximum payload size

struct sockaddr_nl src_addr, dest_addr;  // Source and destination addresses for netlink communication
struct nlmsghdr *nlh = NULL;             // Netlink message header
struct iovec iov;                        // Structure for scatter/gather I/O
int sock_fd;                             // Socket file descriptor
struct msghdr msg;                       // Message header structure

int main() {
    // Establish a socket connection for netlink communication
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
        return -1;

    // Set up source address
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    // Set up destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    // Allocate memory for netlink message header
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    // Set message data
    strcpy(NLMSG_DATA(nlh), "TEST DATA 1");

    // Set up scatter/gather I/O
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // Send message to kernel
    printf("Sending message to kernel\n");
    sendmsg(sock_fd, &msg, 0);
    printf("Waiting for message from kernel\n");

    // Receive message from kernel
    recvmsg(sock_fd, &msg, 0);
    printf("Received message payload: %s\n", NLMSG_DATA(nlh));
    close(sock_fd);
}

