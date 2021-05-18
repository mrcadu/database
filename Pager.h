//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_PAGER_H
#define DATABASE_PAGER_H
#define TABLE_MAX_PAGES 100

#include <stdint.h>

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;


#endif //DATABASE_PAGER_H
