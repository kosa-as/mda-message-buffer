//
// Created by 51275 on 25-1-9.
//
#include "Message.h"

bool Message::operator<(const Message& other) const {
    return this->priority < other.priority;
}
