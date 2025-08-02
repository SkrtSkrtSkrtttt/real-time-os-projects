#include <stdio.h>
#include <stdlib.h>

//Naafiul Hossain
//ESE 333-115107623
struct node
{
    struct node* next;
    int data;
};

struct node* insert(struct node* list, int value)
{
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->data = value;
    newNode->next = NULL;

    if (!list || value < list->data) {
        newNode->next = list;
        list = newNode;
        return list;
    }

    struct node* current = list;
    while (current->next && current->next->data < value) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
    return list;
}

struct node* deleteNode(struct node* list, int value)
{
    if (!list) {
        return NULL;
    }

    if (list->data == value) {
        struct node* temp = list;
        list = list->next;
        free(temp);
        return list;
    }

    struct node* current = list;
    while (current->next && current->next->data != value) {
        current = current->next;
    }

    if (current->next && current->next->data == value) {
        struct node* temp = current->next;
        current->next = current->next->next;
        free(temp);
    }
    return list;
}

int main()
{
    struct node* head = (struct node*)malloc(sizeof(struct node));
    head->data = 0;
    head->next = (struct node*)malloc(sizeof(struct node));
    struct node* current = head->next;
    for (int idx = 2; idx < 10; idx += 2)
    {
        current->data = idx;
        current->next = (struct node*)malloc(sizeof(struct node));
        current = current->next;
    }
    current->data = 10;
    current->next = NULL;

    current = deleteNode(current, 0);
    current = insert(current, -3);
    current = insert(current, 3);
    current = deleteNode(current, 10);
    current = insert(current, 12);
    current = deleteNode(current, 2);
    current = deleteNode(current, 7);

    while (current != NULL)
    {
        printf("found element number %d \n", current->data);
        head = current;
        current = current->next;
        free(head);
    }

    return 0;
}

