#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/poll.h>

void SIGUSR_signal_handler(int sig_number)
{
        printf("I receive SIGABRT signal!\n");
        fflush(stdout);
        abort();
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    pid_t pid;
    char task[3][64] = {"Убери комнату", "Подними фантик","Купи молока"};
    char string[64];
    int fdParentWrite[2], status, flag = 1;
    int fdChildWrite[2];
    struct pollfd fds[2];

    pipe(fdParentWrite);

    //создаём процессы
    for(int j = 0; j < 5; ++j){
        if(flag)
            pid = fork();

        if(pid < 0){
            perror("fork");
            _exit(-1);
        }
        if(pid == 0){
            signal(SIGUSR1, SIGUSR_signal_handler);
            flag = 0;
        }
    }

    //записываем и считываем
    if(pid == 0){
        close(fdParentWrite[1]);

        fds[0].fd = fdParentWrite[0];
        fds[0].events = POLLIN;
        while(1){
            if(poll(fds, 2, 10000) > 0){
                read(fdParentWrite[0], string, sizeof(string));
                printf("%s: %d\n", string, getpid());
                int time = rand()%5 + 1;
                sleep(time);
            }
            else{
                close(fdChildWrite[0]);
                char temp[32];
                snprintf(temp, sizeof (int), "%d", getpid());
                write(fdChildWrite[1], temp, strlen(temp) + 1);
                break;
            }
        }
        _exit(0);
    }

    else {
        close(fdParentWrite[0]);
        for(int i = 0; i < 3; ++i){
            write(fdParentWrite[1], task[i], strlen(task[i])+1);
            sleep(3);
        }
        close(fdChildWrite[1]);
        fds[1].fd = fdChildWrite[0];
        fds[1].events = POLLIN;
        if(poll(fds, 2, -1) > 0){
            char buff[32];
            int chPid;
            read(fdChildWrite[0], buff, sizeof (buff));
            chPid = atoi(buff);
            kill(chPid,SIGUSR1);
        }
    }
    return 0;
}

