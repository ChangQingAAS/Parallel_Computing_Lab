#include <pthread.h>

double N;
int Thread_Num;
double pi;
pthread_mutex_t mut;

//积分法求π线程入口函数,
void *PI_By_Integration(void *arg)
{
    int id = *(int *)arg;
    int length = N / Thread_Num;
    int start = id * length;
    int end = start + length;
    double local_pi = 0.0;
    int count;
    for (count = start; count < end; count++)
    {
        local_pi += (4.0 / (1.0 + ((count + 0.5) / N) * ((count + 0.5) / N))) / N;
    }
    pthread_mutex_lock(&mut);
    pi += local_pi; //将每个线程的计算结果进行累加；
    pthread_mutex_unlock(&mut);

    return NULL;
}