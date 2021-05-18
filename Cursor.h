//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_CURSOR_H
#define DATABASE_CURSOR_H

#include <stdint.h>
#include "Table.h"
#include <stdbool.h>

typedef struct {
    Table* table;
    uint32_t row_num;
    bool end_of_table;
} Cursor;

Cursor* tableStart(Table* table);
Cursor* tableEnd(Table* table);
void* cursorValue(Cursor* cursor);
void cursorAdvance(Cursor* cursor);



#endif //DATABASE_CURSOR_H
