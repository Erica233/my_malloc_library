#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int s = 0;

void * test(void * args) {
    for (int i = 0; i < 10000; i++) {
        s++;
    }
    return NULL;
}

int main() {
    pthread_t th1;
    pthread_t th2;
    pthread_create(&th1, NULL, test, NULL);
    pthread_create(&th2, NULL, test, NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("s = %d\n", s);
    return 0;
}