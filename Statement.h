//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_STATEMENT_H
#define DATABASE_STATEMENT_H

#include "Row.h"

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_SELECTBYCODE, STATEMENT_SELECTBYNAME, STATEMENT_SELECTBYEMAIL, STATEMENT_DELETE, STATEMENT_EXIT } StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;  // only used by insert statement
} Statement;


#endif //DATABASE_STATEMENT_H
