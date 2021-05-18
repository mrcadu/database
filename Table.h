//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_TABLE_H
#define DATABASE_TABLE_H
#define TABLE_MAX_PAGES 100

#include <stdint.h>
#include "Row.h"
#include "Pager.h"
#include <malloc.h>

typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;

uint32_t getTableMaxRows();
uint32_t getRowsPerPage();
uint32_t getRowSize();
Table* getTable(const char* filename);
uint32_t getTableMaxPages();
Pager* pager_open(const char* filename);
void free_table(Table* table);
void* getPage(Pager* pager, uint32_t page_num);
uint32_t getPageSize();

#endif //DATABASE_TABLE_H
