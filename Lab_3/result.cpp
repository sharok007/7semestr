#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fstream>
#include "sharedm.h"

using namespace std;

void error(string err){
    cout << err << endl;
    exit(-1);
}

int main()
{
    key_t keyShared, keySemaphor;
    SharedMem *shared;
    int shmid, semid;

    keyShared = ftok("memory", 'A');
    keySemaphor = ftok("semaphor", 'A');

    ofstream out;
    out.open("result.txt", ios::out);

    //Семафор
    semid = semget(keySemaphor, 1, 0);
    if(semid < 0)
        error("Can not get semaphore");

    //Разделяемая память
    shmid = shmget(keyShared, sizeof (memoryBuffer), 0666);
    if(shmid < 0)
        error("Can not get shared memory segment");

    shared = static_cast<memoryBuffer*>(shmat(shmid, 0, 0));
    if(shared == NULL)
        error("Shared memory attach error");

    while(shared->msg_type != FINISH_MEM){
        if (shared->msg_type == WRITE_STR && semctl(semid, 0, GETVAL, 0) == 0){
            //Блокируем доступ к разделяемой памяти
            semctl (semid, 0, SETVAL, 1);
            //Считываем результат из памяти и записываем его в файлик
            if(shared->msg_type == WRITE_STR){
                out << shared->buff << " = " << shared->result << "\n";
                snprintf(shared->buff, BUFF_SIZE, "");
                shared->result = 0.0;
                shared->msg_type = EMPTY_STR_MEM;
            }
            //Снимаем блокировку
            semctl(semid, 0, SETVAL, 0);
        }
        else
            continue;
    }

    out.close();

    if (semctl (semid, 0, IPC_RMID, static_cast<struct semid_ds *>(0)) < 0)
        error("Semaphore remove error");

    shmdt(shared);

    if(shmctl(shmid,IPC_RMID,NULL) < 0)
        error("Shared memory remove error");

    return 0;
}
