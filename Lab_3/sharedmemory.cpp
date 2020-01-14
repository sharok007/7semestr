#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <cstring>
#include <cstdlib>
#include "sharedm.h"
#include "queuemessage.h"

using namespace std;

void error(string err){
    cout << err << endl;
    exit (-1);
}

int main()
{
    Message msg;
    SharedMem *shared;
    key_t keyQueue, keyShared, keySemaphor;

    int msgid, shmid, semid;

    keyQueue = ftok("server", 'A');
    keyShared = ftok("memory", 'A');
    keySemaphor = ftok("semaphor", 'A');

    //Очередь
    msgid = msgget(keyQueue, 0444);
    if(msgid < 0)
        error("Can not create msg queue" );

    //Семафор
    semid = semget(keySemaphor, 1, 0444 | IPC_CREAT);
    if(semid < 0)
        error("Can not get semaphore");

    //Разделяемая память
    shmid = shmget(keyShared, sizeof(memoryBuffer), 0666 | IPC_CREAT);
    if(shmid < 0)
        error("Can not get shared memory segment");

    shared = static_cast<memoryBuffer*> (shmat(shmid, 0, 0));
    if(shared == NULL)
        error("Shared memory attach error");

    semctl(semid, 0, SETVAL, 0);
    shared->msg_type = EMPTY_STR_MEM;

    while(shared->msg_type != FINISH_MEM){
        if(shared->msg_type == EMPTY_STR_MEM && semctl(semid, 0, GETVAL, 0) == 0){

            //Получаем сообщение
            if(msgrcv(msgid, &msg, sizeof(msg), 0, 0) < 0)
                error("Recive error");
            //Блокируем доступ к разделяемой памяти
            semctl(semid, 0, SETVAL, 1);

            //Записываем в разделяемую память
            if(msg.msg_type == SEND_STR){
                cout << "RECEIVED MESSAGE = " << msg.buff << endl;
                strcpy(shared->buff, msg.buff);
                shared->msg_type = RECEIVE_STR;
                strcpy(shared->status, "READ");
                cout << "STATUS: " << shared->status << endl;
            }
            if(msg.msg_type == FINISH)
                shared->msg_type = FINISH_MEM;
            //Снимаем блокировку
            semctl(semid, 0, SETVAL, 0);
        }
        else
            continue;
    }

    if(msgctl (msgid, IPC_RMID, static_cast<struct msqid_ds *> (0)) < 0)
       error("Queue remove error");

    shmdt(shared);

    return 0;
}
