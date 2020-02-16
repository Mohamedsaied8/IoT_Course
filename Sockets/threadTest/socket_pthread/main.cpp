#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


using namespace std;

// Prints x's to stderr. The parameter is unused. Does not return.
void *print_xs (void* unused)
{
    while (1){
        fputc ('x', stderr);
        usleep(10000);
    }
}

void* print_100(void* unused){
    int i = 100;
    while(i){
        printf("%d",i--);
        usleep(10000);
    }
}

int main ()
{

    pthread_t thread_id;
    pthread_t print_100_thread;
   // pthread_create (&thread_id, NULL,&print_xs, 0);
    pthread_create (&print_100_thread, NULL,&print_100, 0);

    pthread_join(thread_id,NULL);
    pthread_join(print_100_thread, NULL);

    return 0;
}
