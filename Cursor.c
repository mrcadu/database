//
// Created by ceduardo on 18/05/2021.
//
#include <stdbool.h>
#include "Cursor.h"

Cursor* tableStart(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);
    return cursor;
}

Cursor* tableEnd(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;

    return cursor;
}

void* cursorValue(Cursor* cursor) {
    uint32_t row_num = cursor->row_num;
    uint32_t page_num = row_num / getRowsPerPage();
    void* page = getPage(cursor->table->pager, page_num);
    uint32_t row_offset = row_num % getRowsPerPage();
    uint32_t byte_offset = row_offset * getRowSize();
    return page + byte_offset;
}

void cursorAdvance(Cursor* cursor) {
    cursor->row_num += 1;
    if (cursor->row_num >= cursor->table->num_rows) {
        cursor->end_of_table = true;
    }
}
#include "Cursor.h"
