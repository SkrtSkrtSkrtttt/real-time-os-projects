//Naafiul Hossain

//Part 2 publish.c

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

char line[MAX_PAYLOAD];                  // Input line buffer
char tmpLine[MAX_PAYLOAD];               // Temporary line buffer for string manipulation

// Function to delete all occurrences of a character from a string
void deleteChar(char *str, char c) {
    // If the string is null, do nothing.
    // Otherwise, continue with deletion.
    if (str != NULL) {
        int i, j;
        int len = strlen(str);
        for (i = j = 0; i < len; i++) {
            if (str[i] != c) {
                str[j++] = str[i];
            }
        }
        str[j] = '\0';
    }
}

int main() {
    while (1) { // Infinite loop for continuous operation
        fputs("Are you operating as a [p]ublisher or a [s]ubscriber: \n", stdout); // Prompt user for operation type
        fgets(line, MAX_PAYLOAD, stdin); // Read user input
        
        // Remove the newline character at the end of the line
        deleteChar(line, '\n');

        if (line[0] == 'p') { // If operating as publisher
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

            // Set message data to "p" to verify publisher status
            strcpy(NLMSG_DATA(nlh), "p");

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

            while (1) { // Continuous loop for sending data
                fputs("What data would you like to send?\n", stdout);
                fgets(line, MAX_PAYLOAD, stdin); // Read user input
                
                // Remove the newline character at the end of the line
                deleteChar(line, '\n');

                // Automatically add the first character as "p" to verify publisher status
                strcpy(tmpLine, "p");
                strcat(tmpLine, line);
                strcpy(line, tmpLine);

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
                strcpy(NLMSG_DATA(nlh), line);

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

        } else if (line[0] == 's') { // If operating as subscriber
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

            // Set message data to "s" to verify subscriber status
            strcpy(NLMSG_DATA(nlh), "s");

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

            while (1) { // Continuous loop for receiving data
                /* Receive message from kernel */
                recvmsg(sock_fd, &msg, 0);
                printf("Received message payload: %s\n", NLMSG_DATA(nlh));
            }

        } else if (line[0] == 'q') {
            // Handle quit operation if needed
        }
    }
}

