//
// Created by ceduardo on 18/05/2021.
//
#include <unistd.h>
#include "Table.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint32_t PAGE_SIZE = 4096;

uint32_t getRowSize(){
    return getIDSize() + getUsernameSize() + getEmailSize();
}
uint32_t getPageSize(){
    return PAGE_SIZE;
}
uint32_t getRowsPerPage(){
    return PAGE_SIZE / getRowSize();
}
uint32_t getTableMaxRows(){
    return getRowsPerPage() * TABLE_MAX_PAGES;
}
uint32_t getTableMaxPages(){
    return TABLE_MAX_PAGES;
}

Table* getTable(const char* filename) {
    Pager* pager = pager_open(filename);
    uint32_t num_rows = pager -> file_length / getRowSize();
    Table* table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;
    return table;
}
void* getPage(Pager* pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }
    if (pager->pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;
        // We might save a partial page at the end of the file
        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }
        if (page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }
    return pager->pages[page_num];
}
Pager* pager_open(const char* filename) {
    int fd = open(filename,
                  O_RDWR |      // Read/Write mode
                  O_CREAT,  // Create file if it does not exist
                  S_IWUSR |     // User write permission
                  S_IRUSR   // User read permission
    );
    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }
    off_t file_length = lseek(fd, 0, SEEK_END);
    Pager* pager = malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}
