//Publish.c Part 3
//Naafiul Hossain
//ESE33 Publish.C Part 3

//Publish.c Part 3

#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 // maximum payload size

// Structs for Netlink communication
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL; // Netlink message header
struct iovec iov;
int sock_fd;
struct msghdr msg;

pthread_t tid[2];
// Function to delete a specific character from a string
void deleteChar(char *str, char c) {
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

// Subscriber thread: receives messages from the kernel
void *subscriber(void *vargp) {
    while (1) {
        recvmsg(sock_fd, &msg, 0); // Read message from kernel
        printf("Received message from kernel: %s\n", (char *)NLMSG_DATA(nlh));
        memset(NLMSG_DATA(nlh), '\0', sizeof(NLMSG_DATA(nlh))); // Clear memory
    }
}

// Publisher thread: sends messages to the kernel
void *publisher(void *vargp) {
    // Clear memory for NLMSG_DATA(nlh)
    memset(NLMSG_DATA(nlh), '\0', sizeof(NLMSG_DATA(nlh)));

    while (1) {
        char line[MAX_PAYLOAD];
        fputs("Enter data to send:\n", stdout);
        fgets(line, MAX_PAYLOAD, stdin); // Read input data
        line[strcspn(line, "\n")] = '\0'; // Remove newline character

        // Copy input data to nlh
        strcpy(NLMSG_DATA(nlh), line);
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD); // Set message length

        // Prepare message for sending
        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        // Send message
        printf("Sending message to kernel: %s\n", line);
        sendmsg(sock_fd, &msg, 0);
    }
}

int main() {
    // Establish Netlink connection
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
        return -1;
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); // Set PID
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // Kernel PID
    dest_addr.nl_groups = 0; // unicast
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    // Subscribe to kernel
    strcpy(NLMSG_DATA(nlh), "subscribe");
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    printf("Establishing connection with Kernel...\n");
    sendmsg(sock_fd, &msg, 0);

    // Create publisher and subscriber threads
    pthread_create(&tid[0], NULL, &publisher, NULL);
    pthread_create(&tid[1], NULL, &subscriber, NULL);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    close(sock_fd);
}

