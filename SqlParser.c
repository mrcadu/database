//
// Created by ceduardo on 18/05/2021.
//
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "SqlParser.h"
#include "Table.h"
#include "Cursor.h"
#include<ctype.h>

PrepareResult prepare_statement(InputBuffer* input_buffer,Statement* statement) {
    statement->row_to_insert.id = 0;
    strcpy(statement->row_to_insert.email, "");
    strcpy(statement->row_to_insert.username, "");

    statement->current.id = 0;
    strcpy(statement->current.email, "");
    strcpy(statement->current.username, "");

    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3) {
             return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strstr(input_buffer->buffer, "select")) {
        char temp[32];
        sscanf(input_buffer->buffer, "select %s", temp);
        if(isdigit(temp[0])){
            statement->row_to_insert.id = atoi(temp);
            statement->type = STATEMENT_SELECTBYCODE;
        }else if (strstr(input_buffer->buffer, "@")){
            sscanf(input_buffer->buffer, "select %s", statement->row_to_insert.email);
            statement->type = STATEMENT_SELECTBYEMAIL;
        }else{
            sscanf(input_buffer->buffer, "select %s", statement->row_to_insert.username);
            if(strcmp(statement->row_to_insert.username, "") != 0)
            {
                statement->type = STATEMENT_SELECTBYNAME;
            }else{
                statement->type = STATEMENT_SELECT;
            }
        }
        return PREPARE_SUCCESS;
    }

    if (strstr(input_buffer->buffer, "delete")) {

        if (strstr(input_buffer->buffer, "name")) {
            char name[32];
            sscanf(input_buffer->buffer, "delete name %s", name);
            strcpy(statement->current.username,name);

        } else {
            char temp[32];
            sscanf(input_buffer->buffer, "delete %s", temp);
            if (isdigit(temp[0])) {
                statement->current.id = atoi(temp);
            }
        }
        statement->type = STATEMENT_DELETE;
        return PREPARE_SUCCESS;
    }

    if (strstr(input_buffer->buffer, "update")) {
        if (strstr(input_buffer->buffer, "name")) {
            sscanf(input_buffer->buffer, "update name %s %s", statement->current.username, statement->row_to_insert.username);
            statement->type = STATEMENT_UPDATE_NAME;
        }
        else {
            sscanf(input_buffer->buffer, "update email %s %s", statement->current.email, statement->row_to_insert.email);
            statement->type = STATEMENT_UPDATE_EMAIL;
        }
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "exit") == 0) {
        statement->type = STATEMENT_EXIT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_statement(Statement *statement, Table* table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
        case (STATEMENT_SELECTBYCODE):
            return execute_selectByCode(statement, table);
        case (STATEMENT_SELECTBYNAME):
            return execute_selectByName(statement, table);
        case (STATEMENT_SELECTBYEMAIL):
            return execute_selectByEmail(statement, table);
        case (STATEMENT_DELETE):
            return execute_delete(statement, table);
        case (STATEMENT_UPDATE_NAME):
            return execute_update(statement, table);
        case (STATEMENT_UPDATE_EMAIL):
            return execute_update(statement, table);
        case STATEMENT_EXIT:
            closeDB(table);
            break;
    }
    return EXECUTE_FAILED;
}

void closeDB(Table* table) {
    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / getRowsPerPage();

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        flushPages(pager, i, getPageSize());
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    uint32_t num_additional_rows = table->num_rows % getRowsPerPage();
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            flushPages(pager, page_num, num_additional_rows * getRowSize());
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
    exit(EXIT_SUCCESS);
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);
    Row* row_to_insert = &(statement->row_to_insert);
    Row row;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);
        if(row.id == row_to_insert->id){
            row_to_insert->id = table->num_rows + 1;
            Cursor* cursorEnd = tableEnd(table);
            statement->countAccess++;
            serialize_row(row_to_insert, cursorValue(cursorEnd));
            table->num_rows += 1;
            return EXECUTE_SUCCESS;
        }
        cursorAdvance(cursor);
    }

    Cursor* cursorEnd = tableEnd(table);
    serialize_row(row_to_insert, cursorValue(cursorEnd));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_sort(Statement* statement, Table* table) {
    int count = table->num_rows - 1;
    while (count >= 1) {
        Cursor* cursor = tableStart(table);
        Row current;

        Cursor* cursorNext = tableStart(table);
        Row next;
        while ((cursor->row_num < count)) {
            statement->countAccess++;
            deserialize_row(cursorValue(cursor), &current);
            cursorNext->row_num = cursor->row_num;
            cursorNext->row_num+=1;
            statement->countAccess++;
            deserialize_row(cursorValue(cursorNext), &next);

            if (current.id > next.id) {
                Row *nextRow = &next;
                Row *currentRow = &current;

                statement->countAccess++;
                serialize_row(nextRow, cursorValue(cursor));
                statement->countAccess++;
                serialize_row(currentRow, cursorValue(cursorNext));
            }
            cursorAdvance(cursor);
        }
        count--;
        free(cursor);
        free(cursorNext);
    }
    return EXECUTE_SUCCESS;
}

void flushPages(Pager* pager, uint32_t page_num, uint32_t size) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }
    off_t offset = lseek(pager->file_descriptor, page_num * getPageSize(), SEEK_SET);
    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
    if (bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);
    Row row;
    statement->countAccess = 0;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);
        print_row(&row);
        cursorAdvance(cursor);
    }
    free(cursor);
    printf("numero de acessos: %d \n", statement->countAccess);
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_selectByCode(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);
    Row row;
    statement->countAccess = 0;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);
        if(statement->row_to_insert.id == row.id)
        {
            print_row(&row);
        }
        cursorAdvance(cursor);
    }
    free(cursor);
    printf("numero de acessos: %d \n", statement->countAccess);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_selectByName(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);
    Row row;
    statement->countAccess = 0;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);
        if(strstr(row.username, statement->row_to_insert.username) || strstr(statement->row_to_insert.username, row.username))
        {
            print_row(&row);
        }
        cursorAdvance(cursor);
    }
    free(cursor);
    printf("numero de acessos: %d \n", statement->countAccess);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_selectByEmail(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);
    Row row;
    statement->countAccess = 0;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);
        if(strstr(row.email, statement->row_to_insert.email) || strstr(statement->row_to_insert.email, row.email))
        {
            print_row(&row);
        }
        cursorAdvance(cursor);
    }
    free(cursor);
    printf("numero de acessos: %d \n", statement->countAccess);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_delete(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);

    Row row;
    int count = 0;
    statement->countAccess = 0;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);

        if(strcmp(statement->current.username, "")) {
            if(strstr(statement->current.username, row.username) || strstr(row.username, statement->current.username))
            {
                statement->countAccess++;
                deserialize_row(cursorValue(cursor), &row);
                row.id = -1;
                count++;
                Row* rowEnd = &row;
                statement->countAccess++;
                serialize_row(rowEnd, cursorValue(cursor));
            }
        } else{
            if(statement->current.id == row.id)
            {
                statement->countAccess++;
                deserialize_row(cursorValue(cursor), &row);
                row.id = -1;
                count++;
                Row* rowEnd = &row;
                statement->countAccess++;
                serialize_row(rowEnd, cursorValue(cursor));


                printf("numero de acessos: %d \n", statement->countAccess);
                //execute_sort(statement, table);
                //table->num_rows-=count;
                //execute_sort(statement, table);

                return EXECUTE_SUCCESS;
            }
        }
        cursorAdvance(cursor);
    }


    printf("numero de acessos: %d \n", statement->countAccess);
    //execute_sort(statement, table);
    //table->num_rows-=count;
    //execute_sort(statement, table);

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_update(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);
    Row row;
    statement->countAccess = 0;
    while (!(cursor->end_of_table)) {
        statement->countAccess++;
        deserialize_row(cursorValue(cursor), &row);
        if(strcmp(statement->row_to_insert.username, "") == 0){
            if(strstr(row.email, statement->current.email) || strstr(statement->current.email, row.email))
            {
                strcpy(row.username, statement->row_to_insert.email);
                Row* rowUpdate = &row;
                statement->countAccess++;
                serialize_row(rowUpdate, cursorValue(cursor));
            }

        }else if (strcmp(statement->row_to_insert.email, "") == 0){
            if(strstr(row.username, statement->current.username) || strstr(statement->current.username, row.username))
            {
                strcpy(row.username, statement->row_to_insert.username);
                Row* rowUpdate = &row;
                statement->countAccess++;
                serialize_row(rowUpdate, cursorValue(cursor));
            }
        }
        cursorAdvance(cursor);
    }
    free(cursor);
    printf("numero de acessos: %d \n", statement->countAccess);
    return EXECUTE_SUCCESS;
}

void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
