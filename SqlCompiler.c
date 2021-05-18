//
// Created by ceduardo on 18/05/2021.
//
#include "SqlCompiler.h"

void readSql(InputBuffer* sql, Table* table){
    print_prompt();
    read_input(sql);
    Statement statement;
    switch (prepare_statement(sql, &statement)) {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized keyword at start of '%s'.\n",sql->buffer);
            break;
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax Error.\n");
            break;
    }
    switch (execute_statement(&statement, table)) {
        case (EXECUTE_SUCCESS):
            printf("Executed.\n");
            break;
        case EXECUTE_DUPLICATE_KEY:
            printf("Duplicated Key\n");
            break;
        case EXECUTE_FAILED:
            printf("Fail to execute\n");
            break;
    }
}

void close_input_buffer(InputBuffer *sql) {
    free(sql->buffer);
    free(sql);
}

void read_input(InputBuffer *sql) {
    ssize_t bytes_read =
            getline(&(sql->buffer), &(sql->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    sql->input_length = bytes_read - 1;
    sql->buffer[bytes_read - 1] = 0;
}

void print_prompt() {
    printf("db > ");
}
