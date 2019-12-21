#ifndef SHAREDM_H
#define SHAREDM_H

#include <sys/shm.h>
#include <sys/sem.h>

#define BUFF_SIZE 256

enum{
    EMPTY_STR_MEM = 0,
    RECEIVE_STR,
    WRITE_STR,
    FINISH_MEM
};

typedef struct memoryBuffer{
    long msg_type;
    char buff[BUFF_SIZE];
    double result;
}SharedMem;

#endif // SHAREDM_H
