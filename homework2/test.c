#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int s = 0;
__thread int _s = 0;

void * test(void * args) {
    for (int i = 0; i < 10000; i++) {
        pthread_mutex_lock(&lock);
        _s++;
        pthread_mutex_unlock(&lock);
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
    //printf("_s = %d\n", _s);
    return 0;
}