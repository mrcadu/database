//
// Created by ceduardo on 18/05/2021.
//

#ifndef DATABASE_INPUTBUFFER_H
#define DATABASE_INPUTBUFFER_H
#include <stdio.h>

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer *new_input_buffer();

#endif //DATABASE_INPUTBUFFER_H
