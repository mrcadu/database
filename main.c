#include <stdio.h>
#include <stdbool.h>
#include "SqlCompiler.h"
#include "InputBuffer.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
       printf("Must supply a database filename.\n");
       exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    Table* table = getTable(filename);
    while(true){
        InputBuffer* input_buffer = new_input_buffer();
        readSql(input_buffer, table);
    }
    return 0;
}
