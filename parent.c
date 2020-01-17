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

int parsStrTime(char *buf, int j){
	char tempTime[3];
	int time, indexTempTime = 0;
	
	if(j != strlen(buf) - 1){
		for(int i = strlen(buf) - 1; i > j; --i){
			if(buf[i] != ' ' && buf[i] != ':'){
				tempTime[indexTempTime] = buf[i];
				indexTempTime++;
			}
			else
				break;
		}
	}

	if(strlen(tempTime) > 0){
		char temp;
		for (int i = 1; i < strlen(tempTime); ++i){
			temp = tempTime[i - 1];
			tempTime[i - 1] = tempTime[i];
			tempTime[i] = temp;
		}

		time = atoi(tempTime);
	}
	else
		time = 10;
	
	return time;
}

int main(int argc, char *argv[])
{

	srand(time(NULL));

	pid_t pid;
	char task[7][64] = {"Убери комнату:2", "Подними фантик:3","Купи молока: 8", "Помой пол: 3","Полей цветы: 2", "Подмети зал: 7", "Протри пыль: 2"};
	int countTask;
	int countProcess; 
	int childNumber, numberTask = 0;
	
	printf("Введите количество процессов: ");
	scanf("%d", &countProcess);
	printf("Введите количество заданий: ");
	scanf("%d", &countTask);

	//pipe
	int fdChild[countProcess][2];
	int fdParent[countProcess][2];
	
	//poll
	struct pollfd fds[countProcess][1];

	//создаём файловые дескрипторы
	for (int i = 0; i < countProcess; ++i)
	{
		pipe(fdChild[i]);
		pipe(fdParent[i]);
	}

    //создаём процессы
	for(int i = 0; i < countProcess; ++i){
		
		pid = fork();

		if(pid < 0){
			perror("fork");
			_exit(-1);
		}
		if(pid == 0){
			signal(SIGUSR1, SIGUSR_signal_handler);
			childNumber = i;
			break;
		}
	}

    //ребёнок читает задание, выполняет
	if(pid == 0){
		close(fdChild[childNumber][1]);
		char taskChild[64];
		
		while(1){
				read(fdChild[childNumber][0], taskChild, sizeof(taskChild));
				if(taskChild[0] == '-' && taskChild[1] == 'e'){
					close(fdParent[childNumber][0]);
					char temp[32];
					snprintf(temp, sizeof (temp), "%d", getpid());
					write(fdParent[childNumber][1], temp, strlen(temp) + 1);
					close(fdParent[childNumber][1]);
					continue;
				}
				char str[64];
				int j, time;
				for(j = 0; j < strlen(taskChild); ++j){
					if(taskChild[j] != ':')
						str[j] = taskChild[j];

					else
						break;
				}

				time = parsStrTime(taskChild, j);
				printf("PID: %d %s %d sec\n", getpid(), str, time); 
				sleep(time);
		}
		_exit(0);
	}
	//родитель даёт задания
	else {
		printf("PARENT: %d\n", getpid());
		int flag = 0;
		int timeWhile = 0;
		while(1){
			//есть задания, раздаём их
			if(!flag){
				for(int i = 0; i < countProcess; ++i){;
					close(fdChild[i][0]);
					int taskN = rand()%7;
					write(fdChild[i][1], task[taskN], strlen(task[taskN])+1);
					numberTask++;
					sleep(3);
					if(numberTask == countTask){
						for(int j = 0; j < countProcess; ++j){
						write(fdChild[j][1], "-e", 4);
						}
						flag = 1;
						break;
					}
				}
			}
			//задания закончились, ждём pid ребёнка
			else{
				for(int i = 0; i < countProcess; ++i){
					close(fdChild[i][1]);
					close(fdChild[i][0]);

					close(fdParent[i][1]);
					fds[i][0].fd = fdParent[i][0];
					fds[i][0].events = POLLIN;
					if(poll(fds[i], 1, 15000) > 0){
						char pidCh[64];
						read(fdParent[i][0], pidCh, sizeof(pidCh));
						int children = atoi(pidCh);
						printf("I'm kill %s\n", pidCh);
						kill(children, SIGUSR1);
						timeWhile++;
					}						
				}
			}

			if(timeWhile == countProcess)
				break;
		}
	}

	return 0;
}

