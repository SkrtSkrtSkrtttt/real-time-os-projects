//PubSub.c Part 3 Naafiul Hossain
#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#define NETLINK_USER 31

#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>

struct sock *nl_sk = NULL;
int user_pid;

struct linkedList {
    int process_id;
    struct list_head list;
};

LIST_HEAD(subscribers);

void removeFirstCharacter(char *str) {
    memmove(str, str + 1, strlen(str));
}

static void handle_nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh;
    struct sk_buff *skb_out;
    int message_size;
    char *message = "\0";
    int result;
    int found = 1;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "Netlink received message payload: %s\n", (char *)nlmsg_data(nlh));

    user_pid = nlh->nlmsg_pid;
    printk(KERN_INFO "Netlink received from PID: %d\n", user_pid);

    char received_message[strlen((char *)nlmsg_data(nlh))];
    strcpy((char *)received_message, (char *)nlmsg_data(nlh));
    printk(KERN_INFO "Value: %s\n", received_message);
    struct linkedList *ptr;
    struct linkedList *temp;

    if (strcmp(received_message, "subscribe") == 0) {
        printk("Subscribe operation: %d\n", user_pid);

        found = 0;

        list_for_each_entry(ptr, &subscribers, list) {
            printk("List currently - PID: %d\n", ptr->process_id);
            if (ptr->process_id == user_pid) {
                printk("Found matching PID: %d\n", ptr->process_id);
                found = 1;
            } else {
                printk("Non-matching PID: %d\n", ptr->process_id);
            }
        }
    }

    if (found == 0) {
        temp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
        temp->process_id = user_pid;
        list_add_tail(&temp->list, &subscribers);
        printk(KERN_INFO "Added PID: %d to the linked list\n", user_pid);
    } else {
        found = 0;

        message = kmalloc(sizeof((char *)nlmsg_data(nlh)), GFP_KERNEL);
        strcpy(message, (char *)nlmsg_data(nlh));
        message_size = strlen(message);
        printk(KERN_INFO "The message size is: %d\n", message_size);

        list_for_each_entry(ptr, &subscribers, list) {
            skb_out = nlmsg_new(message_size, 0);
            if (!skb_out) {
                printk(KERN_ERR "Failed to allocate new skb\n");
                return;
            }
            nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, message_size, 0);
            NETLINK_CB(skb_out).dst_group = 0;
            strncpy(nlmsg_data(nlh), message, message_size);

            printk("Broadcasting to PID: %d\n", ptr->process_id);

            if (nlmsg_unicast(nl_sk, skb_out, ptr->process_id) < 0) {
                printk(KERN_INFO "Error while sending message\n");
            } else {
                printk(KERN_INFO "Message sent\n");
            }
        }
    }
}

static int __init hello_init(void) {
    printk("Entering: %s\n", __FUNCTION__);
    struct netlink_kernel_cfg cfg = {
        .input = handle_nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

    struct linkedList *tmp, *next;
    struct linkedList *ptr = NULL;

    tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
    tmp->process_id = 0;
    list_add_tail(&tmp->list, &subscribers);

    tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
    tmp->process_id = 1;
    list_add_tail(&tmp->list, &subscribers);

    tmp = kmalloc(sizeof(struct linkedList), GFP_KERNEL);
    tmp->process_id = 2;
    list_add_tail(&tmp->list, &subscribers);

    list_for_each_entry(ptr, &subscribers, list) {
        printk("My List - PID: %d\n", ptr->process_id);
    }

    list_for_each_entry_safe(tmp, next, &subscribers, list) {
        list_del(&tmp->list);
        kfree(tmp);
    }

    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "Exiting hello module\n");
    netlink_kernel_release(nl_sk);

    struct linkedList *tmp, *next;

    list_for_each_entry_safe(tmp, next, &subscribers, list) {
        list_del(&tmp->list);
        kfree(tmp);
    }
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


