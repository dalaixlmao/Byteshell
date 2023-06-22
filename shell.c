
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_NUM_ARGS 64
#define MAX_HISTORY_SIZE 100


char* history[MAX_HISTORY_SIZE];
int history_count = 0;


typedef struct HistoryNode {
    char* command;
    struct HistoryNode* next;
} HistoryNode;


HistoryNode* history_head = NULL;



char* read_line();
char** split_line(char* line);
int execute(char** args);
int is_builtin(char* command);
void add_to_history(char* line);
int print_history();
int cmd_alias(char** args);
int cmd_help(char** args);
int cmd_echo(char** args);
int cmd_printf(char** args);
int cmd_readarray(char** args);


int cd(char** args);



char* builtin_commands[] = {
    "cd",
    "alias",
    "help",
    "echo",
    "printf",
    "readarray",
    "history"
    // Add more built-in command names here
};

// Array of corresponding function pointers for built-in commands
int (*builtin_functions[])(char**) = {
    &cd,
    &cmd_alias,
    &cmd_help,
    &cmd_echo,
    &cmd_printf,
    &cmd_readarray,
    &print_history

    // Add more built-in command functions here
};

// Main shell loop
void loop() {
    char* line;
    char** args;
    int status;

    do {
        printf("ByteShell> ");
        line = read_line();
        add_to_history(line);
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
}

// Read a line of input from the user
char* read_line() {
    char* line = malloc(MAX_COMMAND_LENGTH);
    fgets(line, MAX_COMMAND_LENGTH, stdin);

    // Remove newline character at the end, if present
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';

    return line;
}

// Split the command line into arguments
char** split_line(char* line) {
    char** args = malloc(MAX_NUM_ARGS * sizeof(char*));
    char* token;
    int i = 0;

    token = strtok(line, " ");
    while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    return args;
}

// Execute the command
int execute(char** args) {
    if (args[0] == NULL) {
        return 1;  // Empty command, continue shell loop
    }

    // Check if the command is a built-in command
    if (is_builtin(args[0])) {
        int index = 0;
        while (strcmp(args[0], builtin_commands[index]) != 0) {
            index++;
        }
        return (*builtin_functions[index])(args);
    }

    printf("Command not found: %s\n", args[0]);
    return 1;  // Continue shell loop
}


// Check if the command is a built-in command
int is_builtin(char* command) {
    int i;
    for (i = 0; i < sizeof(builtin_commands) / sizeof(char*); i++) {
        if (strcmp(command, builtin_commands[i]) == 0)
            return 1;
    }
    return 0;
}

// Add command to history
void add_to_history(char* line) {
    HistoryNode* new_node = (HistoryNode*)malloc(sizeof(HistoryNode));
    new_node->command = strdup(line);
    new_node->next = NULL;

    if (history_head == NULL) {
        history_head = new_node;
    } else {
        HistoryNode* current = history_head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}


// Print command history
int print_history() {
    HistoryNode* current = history_head;
    int count = 1;

    while (current != NULL) {
        printf("%d: %s\n", count, current->command);
        current = current->next;
        count++;
    }

    return 1;
}

// Free the history linked list
void free_history() {
    HistoryNode* current = history_head;
    while (current != NULL) {
        HistoryNode* temp = current;
        current = current->next;
        free(temp->command);
        free(temp);
    }
}

// Alias command
int cmd_alias(char** args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "alias: missing arguments\n");
    } else {
        // Store the alias in some data structure or perform necessary operations
        // Here, we will simply print the alias for demonstration purposes
        printf("Alias: %s = %s\n", args[1], args[2]);
    }
    return 1;  // Continue shell loop
}

// Help command
int cmd_help(char** args) {
    printf("ByteShell\n");
    printf("A simple shell implementation\n");
    printf("Available built-in commands:\n");
    for (int i = 0; i < sizeof(builtin_commands) / sizeof(char*); i++) {
        printf("- %s\n", builtin_commands[i]);
    }
    return 1;  // Continue shell loop
}

// Echo command
int cmd_echo(char** args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 1;  // Continue shell loop
}

// Printf command
int cmd_printf(char** args) {
    // Concatenate the arguments to form the format string
    char format_string[MAX_COMMAND_LENGTH] = "";
    for (int i = 1; args[i] != NULL; i++) {
        strcat(format_string, args[i]);
        strcat(format_string, " ");
    }
    printf(format_string);
    return 1;  // Continue shell loop
}

// Readarray command
int cmd_readarray(char** args) {
    printf("Enter values (press Ctrl+D to finish):\n");

    char* line = NULL;
    size_t line_length = 0;
    size_t read;

    // Read lines from user input until Ctrl+D is pressed
    while ((read = getline(&line, &line_length, stdin)) != -1) {
        // Remove newline character at the end, if present
        if (line[read - 1] == '\n')
            line[read - 1] = '\0';
        printf("Read: %s\n", line);
    }

    free(line);
    return 1;  // Continue shell loop
}

// Implementation of built-in commands

// Change directory
int cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing directory\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
    return 1;  // Continue shell loop
}

int main() {
    loop();
    return 0;
}
