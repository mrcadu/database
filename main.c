#include <stdio.h>
#include <stdbool.h>
#include "SqlCompiler.h"
#include "InputBuffer.h"

void populateDatabase();

int main(int argc, char* argv[])
{
    if (argc < 2) {
       printf("Must supply a database filename.\n");
       exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    Table* table = getTable(filename);
    populateDatabase(table);
    printf(" database populated! \n");
    while(true){
        InputBuffer* input_buffer = new_input_buffer();
        readSql(input_buffer, table);
    }
    return 0;
}

void populateDatabase(Table* table)
{
    FILE *the_file = fopen("database.csv", "r");
    if(the_file == NULL){
        perror("Unable to open the file");
        exit(1);
    }
    char line[4096];
    Statement statement;
    while(fgets(line, sizeof(line), the_file)){
        char *token;
        token = strtok(line, ",");
        Row row_to_insert;
        int count = 0;

        while(token != NULL){
            switch (count) {
                case 0:
                    row_to_insert.id = atoi(token);
                    count++;
                    break;
                case 1:
                    strcpy(row_to_insert.username, token);
                    count++;
                    break;
                case 2:
                    strcpy(row_to_insert.email, token);
                    break;
            }
            token = strtok(NULL,",");
        }
        statement.row_to_insert = row_to_insert;
        execute_insert(&statement, table);
    }
    printf("numero de acessos: %d \n", statement.countAccess);
    statement.countAccess = 0;
}
