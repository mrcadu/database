//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_ROW_H
#define DATABASE_ROW_H

#include <stdint.h>
#include <string.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute);

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

uint32_t getIDSize();
uint32_t getUsernameSize();
uint32_t getEmailSize();
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);

#endif //DATABASE_ROW_H
