//
// Created by ceduardo on 18/05/2021.
//

#include <malloc.h>
#include "InputBuffer.h"

InputBuffer *new_input_buffer() {
    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}
