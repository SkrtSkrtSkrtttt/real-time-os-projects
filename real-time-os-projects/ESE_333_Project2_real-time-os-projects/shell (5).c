// Naafiul Hossain
// ESE33 115107623 

// Goal of this Project: Implement a basic shell that takes in commands like cd, ls and other commands.
// We implement a linked list to keep track of such commands. The bonus portion of the project was attempted.
// I had some concerns with how I was able to call some of the functions and had trouble asking for user input
// but I think it mostly works.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 64

// Define a structure for linked list node
typedef struct Node {
    char *command;
    struct Node *next;
} Node;

char currentDir[MAX_COMMAND_LENGTH]; // Global variable to store current directory

// Function prototypes
Node *parseInput(char *input);
void freeCommandList(Node *head);
void displayCommands(Node *head);
void execute(Node *head);
void changeDir(char *path);
void listDir();

int main() {
    char input[MAX_COMMAND_LENGTH];

    getcwd(currentDir, sizeof(currentDir)); // Get the current working directory

    while (1) {
        printf("Shell %s> ", currentDir); // Print the shell prompt
        fgets(input, MAX_COMMAND_LENGTH, stdin); // Read user input
        input[strcspn(input, "\n")] = '\0'; // Remove newline character from input

        Node *commands = parseInput(input); // Parse user input into linked list
        execute(commands); // Execute the parsed commands
    }

    return 0;
}

// Parse the user input into linked list of commands
Node *parseInput(char *input) {
    Node *head = NULL;
    Node *current = NULL;
    char *token = strtok(input, " ");

    while (token != NULL) {
        Node *newNode = (Node *)malloc(sizeof(Node)); // Allocate memory for new node
        newNode->command = strdup(token); // Copy token into node command
        newNode->next = NULL; // Set next pointer to NULL

        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }

        token = strtok(NULL, " "); // Get the next token
    }

    return head; // Return the head of the linked list
}

// Execute the commands parsed from input
void execute(Node *head) {
    if (head == NULL)
        return;

    if (strcmp(head->command, "cd") == 0) { // Change directory command
        if (head->next != NULL)
            changeDir(head->next->command);
        else
            fprintf(stderr, "cd: missing argument\n");
    } else if (strcmp(head->command, "ls") == 0) { // List directory command
        listDir();
    } else { // Other commands
        int fd[2]; // File descriptors for pipe
        pid_t pid;

        char *args[MAX_ARGUMENTS]; // Array to hold command and arguments
        int numArgs = 0; // Counter for number of arguments

        // Loop through the commands and build the args array
        while (head != NULL && strcmp(head->command, "&") != 0 && numArgs < MAX_ARGUMENTS - 1) {
            args[numArgs++] = head->command;
            head = head->next;
        }
        args[numArgs] = NULL; // Terminate the args array with NULL

        // Fork a new process
        pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            // Execute the command
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else { // Parent process
            // Wait for the child process to finish
            wait(NULL);
        }
    }
}

// Change the current directory
void changeDir(char *path) {
    if (chdir(path) == -1) { // Attempt to change directory
        perror("chdir");
    } else { // Change successful
        getcwd(currentDir, sizeof(currentDir)); // Update current directory
    }
}

// Free memory allocated for command list
void freeCommandList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp->command); // Free command string
        free(temp); // Free node
    }
}

// List contents of current directory
void listDir() {
    DIR *dir; // Directory stream
    struct dirent *entry;

    if ((dir = opendir(currentDir)) == NULL) { // Open the current directory
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) { // Read directory entries
        printf("%s\n", entry->d_name); // Print each entry
    }

    closedir(dir); // Close the directory stream
}

// Print the commands and arguments
void displayCommands(Node *head) {
    printf("Commands: ");

    Node *current = head;
    while (current != NULL) {
        printf("%s ", current->command);
        current = current->next;
    }

    printf("\n");
}

