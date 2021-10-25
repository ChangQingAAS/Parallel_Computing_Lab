#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "Integration.c"
#include "Monte_Carlo.c"
#include "Power_Series.c"

#define PI acos(-1) //真实π值
#define N_Num 5     //n的个数
#define TN_Num 14   //线程数的个数

double NN[N_Num] = {100000.0, 1000000.0, 10000000.0, 100000000.0, 1000000000.0};
int TN[TN_Num] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
double pi = 0.0;
double N;
int Thread_Num;
float serial_time;
float current_time;
double Imprecision;
float speed_up;
float effciency;

pthread_mutex_t mut;

int main()
{

    int i, j;
    for (i = 0; i < N_Num; i++)
    {
        printf("幂级数法求π值（N=%.1lf)\n", NN[i]);
        printf("N\t\t\tThread_Num\tPI(运算结果)\t\t\tCost Time(ms)\t\tSpeed_up\t\tEffciency(in %)\t\tImprecision(1/10e+11)\n");
        N = NN[i];
        for (j = 0; j < TN_Num; j++)
        {
            Thread_Num = TN[j];

            //判断输入是否合理
            if (Thread_Num > N || Thread_Num <= 0 || N <= 0)
            {
                printf("Wrong input!\n");
                return 0;
            }

            //记录线程开始时间
            struct timeval time_start, time_end;
            gettimeofday(&time_start, NULL);

            //创建线程的准备
            pthread_t *threads;
            threads = (pthread_t *)malloc(sizeof(pthread_t) * Thread_Num); //开辟了thread_num个线程
            int check_create;                                              //判断线程创建成功否
            int check_end;                                                 //判断线程退出成功否
            int x[Thread_Num];
            int pthread_no;

            // 创建线程
            for (pthread_no = 0; pthread_no < Thread_Num; pthread_no++)
            {
                x[pthread_no] = pthread_no; //确定线程是第几个线程
                // T个线程每个线程调用一次线程函数，并传入参数，确定积分范围,传回参数判断是否创建成功
                check_create =
                    pthread_create(&threads[pthread_no], NULL, PI_By_Power_Series, &x[pthread_no]);

                if (check_create != 0)
                {
                    printf("Thread%d creation failed!\n", pthread_no);
                    exit(1);
                }
            }

            // 结束线程
            for (pthread_no = 0; pthread_no < Thread_Num; pthread_no++)
            {
                //等待所有线程结束
                check_end = pthread_join(threads[pthread_no], NULL);

                if (check_end != 0)
                {
                    printf("Thread%d end failed!\n", pthread_no);
                    exit(1);
                }
            }

            //记录线程结束时间，算出线程运行时间、加速比，效率、计算出的pi值得精确度
            gettimeofday(&time_end, NULL);
            current_time = (float)((1000000 * (time_end.tv_sec - time_start.tv_sec) + time_end.tv_usec - time_start.tv_usec) / 1000);
            if (Thread_Num == 1)
            {
                serial_time = current_time;
                // printf("serial time is %.20lf \n", serial_time);
            }
            speed_up = serial_time / current_time;
            effciency = speed_up / Thread_Num * 100;
            Imprecision = fabs(PI - pi) * 100000000000;

            //打印相关数据
            printf("%.1f\t\t", N);
            printf("%d\t\t", Thread_Num);
            printf("%.20f\t\t", pi);
            printf("%0.2f\t\t\t", current_time);
            printf("%.5f\t\t\t", speed_up);
            printf("%.3f\t\t\t", effciency);
            printf("%.5f\n", Imprecision);

            //计算结束后pi值归0，然后重新计算pi值
            pi = 0.0;
        }

        printf("\n");
    }
    return 0;
}
