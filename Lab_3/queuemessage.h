#ifndef QUEUEMESSAGE_H
#define QUEUEMESSAGE_H

#include <sys/msg.h>

#define BUFF_SIZE 256

enum{
    EMPTY_STR = 0,
    SEND_STR,
    FINISH
};

typedef struct mesgBuffer{
    long msg_type;
    char buff[BUFF_SIZE];
    int countEquations;
}Message;

#endif // QUEUEMESSAGE_H
