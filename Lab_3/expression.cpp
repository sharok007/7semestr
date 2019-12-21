#include <sys/types.h>
#include <sys/ipc.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#include "queuemessage.h"

using namespace std;

int main()
{
    srand(time(NULL));

    int count = 3; //Кол-во уравнений
    int enumeration = 0; //Счётчик
    char operations [5] = {'+', '-', '*', '/', '\0'};
    int length = 10; //Кол-во чисел в уравнении
    char temp[BUFF_SIZE];
    temp[0]='\0';

    //Очередь
    Message msg;
    key_t key;
    int msgid;
    size_t  len;
    key = ftok("server", 'A');

    msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid < 0){
        cout << "Can not create msg queue" << endl;
        return -1;
    }

    msg.msg_type = EMPTY_STR;
    msg.countEquations = count;

    while(enumeration != count + 1){
        if(enumeration != count){// Рандомим выражения
            for(int i = 0; i < length; ++i){
                int operation = rand()%4;
                if(i < (length - 1)){
                    snprintf(msg.buff, BUFF_SIZE,"%s%d%c", temp, rand()%100 + 1, operations[operation]);
                }
                else
                    snprintf(msg.buff, BUFF_SIZE,"%s%d", temp, rand()%100 + 1);
                strcpy(temp, msg.buff);
            }
            msg.msg_type = SEND_STR;
        }
        else
            msg.msg_type = FINISH;

        len = sizeof (msg.buff);

        if(msgsnd(msgid, &msg, len, 0) != 0){ //Отправляем сообщения
            cout << "Message send error" << endl;
            return -1;
        }
        else{
            snprintf(msg.buff, BUFF_SIZE, "");
            snprintf(temp, BUFF_SIZE, "");
            msg.msg_type = EMPTY_STR;
        }

        sleep(5);
        enumeration++;
    }

    return 0;
}
