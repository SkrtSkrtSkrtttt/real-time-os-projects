//Naafiul Hossain

//Part 2 pubsub.c

#include <linux/module.h>      // Kernel module definitions
#include <net/sock.h>          // Networking socket functions
#include <linux/netlink.h>     // Netlink communication protocol
#include <linux/skbuff.h>      // Socket buffer definitions
#define NETLINK_USER 31        // Custom netlink protocol identifier

#include <linux/string.h>      // String manipulation functions

struct sock *nl_sk = NULL;      // Netlink socket for communication between kernel and userspace
int pid;

int pid_Sub[2] = {0, 0};        // Subscriber PID array
int pid_Pub = 0;                 // Publisher PID
int i = 0;

// Function to delete the first character of a string
void delete2(char *str) {
    memmove(str, str+1, strlen(str));
}

// This function receives messages from userspace through the netlink socket
static void hello_nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;       // Netlink Message Header
    struct sk_buff *skb_out;    // Pointer to the socket buffer
    int msg_size;
    char *msg = "Hello from kernel"; // Message to send back to the user
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    nlh = (struct nlmsghdr *)skb->data; // Data received from the publisher
    printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));

    pid = nlh->nlmsg_pid; /*pid of publishing process */
    printk(KERN_INFO "Netlink received from PID:%d\n", pid);

    // If the received message begins with 'p'
    if (((char *)nlmsg_data(nlh))[0] == 'p') {
        char recMsg[strlen((char *)nlmsg_data(nlh))];
        strcpy((char *)recMsg, (char *)nlmsg_data(nlh));

        // Set the publisher ID to the current process received
        if (pid_Pub == 0) {
            pid_Pub = pid;
            printk(KERN_INFO "Publisher intilaized as PID: %d\n", pid);
            msg = "You are now a publisher";
        } else if (pid_Pub == pid) {
            delete2(recMsg);
            printk(KERN_INFO "Broadcast value: %s\n", recMsg);
            msg = recMsg;

            for(i = 0; i < 2; i++){
                if (pid_Sub[i] != 0){
                    printk(KERN_INFO "To add to pid: %d\n", pid_Sub[i]);

                    msg_size = strlen(msg);
                    skb_out = nlmsg_new(msg_size, 0);
                    if (!skb_out) {
                        printk(KERN_ERR "Failed to allocate new skb\n");
                        return;
                    }
                    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
                    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
                    strncpy(nlmsg_data(nlh), msg, msg_size);

                    if (nlmsg_unicast(nl_sk, skb_out, pid_Sub[i]) < 0) {
                        printk(KERN_INFO "Error while sending back to user\n");
                    } else {
                        printk(KERN_INFO "Msg sent");
                    }
                }
            }

            printk(KERN_INFO "Broadcasted");

        } else {
            printk(KERN_INFO "Publisher already established as: %d\n", pid_Pub);
            printk(KERN_INFO "Requesting PID is: %d\n", pid);

            msg = "Publisher has already been established";
        }
        printk(KERN_INFO "Information changed succesful");
    } else if (((char *)nlmsg_data(nlh))[0] == 's') { // If the received message begins with 's'
        // Default message is error
        msg = "Subscriber list full or error";

        // Set subscriber ID to the current process received (if possible)
        for (i = 0; i < 2; i++) {
            if (pid_Sub[i] == 0) {
                pid_Sub[i] = pid;
                msg = "You have been subscribed";
                break;
            } else if (pid_Sub[i] == pid) {
                msg = "No need to resubscribe. ";
                break;
            } else {
                msg = "";
                printk(KERN_INFO "%d Subscriber already established as: %d\n", i, pid_Sub[i]);
                printk(KERN_INFO "Requesting PID is: %d\n", pid);
            }
        }
        printk(KERN_INFO "Information send succesful");
    } else {
        printk(KERN_INFO "Did not receive valid value from: %d\n", pid);
        printk(KERN_INFO "Publisher: %d\n", pid_Pub);
        for (i = 0; i < 2; i++) {
            printk(KERN_INFO "Subscriber %d: %d\n", i, pid_Sub[i]);
        }
        msg = "Invalid Request";
    }

    msg_size = strlen(msg);
    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    // nl_sk is the struct sock * returned by netlink_kernel_create
    // skb_out is a buffer that contains the message
    // pid is the pid of the process to which the message should be sent to
    if (nlmsg_unicast(nl_sk, skb_out, pid) < 0) {
        printk(KERN_INFO "Error while sending back to user\n");
    } else {
        printk(KERN_INFO "Msg sent");
    }
}

// Module initialization function
static int __init hello_init(void)
{
    printk("Entering: %s\n", __FUNCTION__);
    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }
    return 0;
}

// Module exit function
static void __exit hello_exit(void)
{
    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(hello_init); 
module_exit(hello_exit);

MODULE_LICENSE("GPL");

