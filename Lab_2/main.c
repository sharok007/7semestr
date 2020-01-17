#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

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

	if(strlen(tempTime) > 1){
		char temp;
		for (int i = 1; i < strlen(tempTime); ++i){
			temp = tempTime[i - 1];
			tempTime[i - 1] = tempTime[i];
			tempTime[i] = temp;
		}
	}

	time = atoi(tempTime);
	return time;
}

void SIGUSR_signal_handler(int sig){
	if(sig == SIGUSR1){
		printf("I'm dead\n" );
		abort();
	}
}

int main(int argc, char const *argv[])
{
	char task[5][64] = {"Убери комнату: 10", "Помой пол: 3", "Полей цветы: 2", "Подмети зал: 7", "Протри пыль: 2"};
	char buf[64];
	pid_t pid;
	int fd[2];
	int status, flag = 1, i=0;
	for (int j = 0; j < 5; ++j)
	{
		printf("%d\n", flag);
		if(flag == 1){
			pid = fork();
		}

		if(pid == 0 && flag == 1){
			flag = 0;
			sleep(10);
			printf("Child %d\n", j);
			//exit(0);
		}
		else if(pid > 0){
			//flag = 1;
			printf("Parent %d\n", j);
		}
		else
			exit(-1);
	}
/*
	pipe(fd);

	pid = fork();
	if(pid < 0)
		exit(-1);

	else if(pid == 0){
		signal(SIGUSR1, SIGUSR_signal_handler);
		
		close(fd[1]);
		read(fd[0], buf, sizeof(buf));

		char str[64];
		int j, time;
		for(j = 0; j < strlen(buf); ++j){
			if(buf[j] != ':')
				str[j] = buf[j];

			else
				break;
		}

		time = parsStrTime(buf, j);
		sleep(time);
	}

	else {
		close(fd[0]);
		write(fd[1], task[i], (strlen(task[i])+1));
		kill(pid, SIGUSR1);
	}
*/
	return 0;
}