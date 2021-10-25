#include <pthread.h>
#include <stdlib.h>

double N;
int Thread_Num;
double pi;
pthread_mutex_t mut;

//线程入口函数
void *PI_By_Monte_Carlo(void *arg)
{
    unsigned int seed = 123;
    int length = N / Thread_Num;
    int temp = 0;
    int i;
    double local_pi = 0.0;
    for (i = 0; i < length; i++)
    {
        double x = 1.0 * rand_r(&seed) / RAND_MAX;
        double y = 1.0 * rand_r(&seed) / RAND_MAX;
        if (x * x + y * y < 1.0)
        {
            temp++;
        }
    }
    local_pi += temp * 4.0 / N;
    pthread_mutex_lock(&mut);
    pi += local_pi;
    pthread_mutex_unlock(&mut);
    return NULL;
}
