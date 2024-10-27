#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int bitsCount = 4;
int acc = 0;
int tmpArr[15];
int size = 0;
int result = 0;

int reverse(int *arr) {

    for (int i = --size; i >= 0; --i) {
        result = result * 10 + arr[i];
    }
    return result;
}

void sigusr1(int signum) {
    acc = (acc << 1) + 0;
}

void sigusr2(int signum) {
    acc = (acc << 1) + 1;
}

void clear(int signum) {
    tmpArr[size++] = acc;
    acc = 0;
}

void retRes(int signum) {
    printf("result = %d \n", reverse(tmpArr));
    signal(SIGINT, SIG_DFL);
    exit(EXIT_SUCCESS);
}

int main() {
    int status;

    pid_t child = fork();

    if (!child) {
        signal(SIGUSR1, sigusr1); // for 0 bit
        signal(SIGUSR2, sigusr2); // for 1 bit
        signal(SIGALRM, clear); // for clear acc
        signal(SIGINT, retRes); // for print result
        while (1);
    } else if (child > 0) {
        int num = 0;
        int tmp = 0;

        puts("Input number");
        scanf("%d", &num);

        while (num) {
            tmp = num % 10;
            num /= 10;

            for (int i = bitsCount - 1; i >= 0; --i) {
                if ((tmp & (1 << i)) == 0) {
                    kill(child, SIGUSR1);
                    usleep(30000);
                } else {
                    kill(child, SIGUSR2);
                    usleep(30000);
                }
            }

            kill(child, SIGALRM);
            usleep(30000);
        }

        kill(child, SIGINT);
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Child process killed normally (status <%d>) \n", WEXITSTATUS(status));
        } else {
            printf("Child process killed abnormally (signum <%d>) \n", WTERMSIG(status));
        }
    }

    return 0;
}
