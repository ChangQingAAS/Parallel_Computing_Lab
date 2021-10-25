#include <pthread.h>

double N;
int Thread_Num;
double pi;
pthread_mutex_t mut;

//幂级数法求π线程入口函数
void *PI_By_Power_Series(void *arg)
{
    //int *income = (int *)arg;
    int id = *(int *)arg;
    int length = N / Thread_Num;
    int start = id * length;
    int end = start + length;
    double local_pi = 0.0;
    double flag; //符号位，标志正负
    int i;
    if (start % 2 == 0)
        flag = 1.0;
    else
        flag = -1.0;
    for (i = start; i < end; i++, flag = -flag)
    {
        local_pi += flag / (2 * i + 1);
    }
    pthread_mutex_lock(&mut);
    pi += 4.0 * local_pi;
    pthread_mutex_unlock(&mut);
    return NULL;
}
