//
// Created by ceduardo on 18/05/2021.
//

#include "Row.h"
static const uint32_t ID_SIZE = size_of_attribute(Row, id);
static const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
static const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

uint32_t getIDSize(){
    return ID_SIZE;
}
uint32_t getUsernameSize(){
    return USERNAME_SIZE;
}
uint32_t getEmailSize(){
    return EMAIL_SIZE;
}
uint32_t getIdOffset(){
    return 0;
}
uint32_t getUsernameOffset(){
    return getIdOffset() + getIDSize();;
}
uint32_t getEmailOffset(){
    return getUsernameOffset() + getUsernameSize();;
}

void serialize_row(Row* source, void* destination) {
    memcpy(destination + getIdOffset(), &(source->id), ID_SIZE);
    memcpy(destination + getUsernameOffset(), &(source->username), USERNAME_SIZE);
    memcpy(destination + getEmailOffset(), &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), source + getIdOffset(), ID_SIZE);
    memcpy(&(destination->username), source + getUsernameOffset(), USERNAME_SIZE);
    memcpy(&(destination->email), source + getEmailOffset(), EMAIL_SIZE);
}