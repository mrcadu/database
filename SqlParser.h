//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_SQLPARSER_H
#define DATABASE_SQLPARSER_H

#include "PrepareResult.h"
#include "Statement.h"
#include "InputBuffer.h"
#include "Table.h"
#include <stdio.h>
#include <string.h>
#include "ExecuteResult.h"

PrepareResult prepare_statement(InputBuffer* input_buffer,Statement* statement);

ExecuteResult execute_statement(Statement *statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_selectByCode(Statement* statement, Table* table);
ExecuteResult execute_selectByName(Statement* statement, Table* table);
ExecuteResult execute_selectByEmail(Statement* statement, Table* table);
ExecuteResult execute_insert(Statement* statement, Table* table);
void flushPages(Pager* pager, uint32_t page_num, uint32_t size);
void closeDB(Table* table);
void print_row(Row* row);

#endif //DATABASE_SQLPARSER_H
