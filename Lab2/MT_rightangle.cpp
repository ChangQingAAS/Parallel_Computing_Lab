#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


//n为矩阵的长度或宽度
int N;
//n的个数
#define N_Num  8 
int NN[N_Num] = {256,512,1024,2048,4096,8192,16384,32768};

int Thread_Num;
//线程数的个数
#define TN_Num 8  
int TN[TN_Num] = {1, 2, 4, 8,16,32,64,128};

//串行计算的时间，即单线程
float serial_time;
//并行计算时间
float current_time;
//加速比
float speed_up;
//效率
float effciency;
//矩阵
int **matrix;
//两个变量相互交换中用到的中间变量
int temp=0;

void *right_angle_division(void *arg)
{
    //确定该线程id
    int id = *(int *)arg;
    //确定线程负责的规模
    int length = N / Thread_Num;
    //确定线程的截至
    int end = (id == Thread_Num - 1) ? N : id + length;
    //确定线程的起始
    int begin = id * length; 
    for(int rank=begin;rank<end;rank++)
    {
    	for(int i=0;i<rank;i++)
        {
	        temp = matrix[rank][i];
	        matrix[rank][i] = matrix[i][rank];
	        matrix[i][rank] = temp;
    	}
	}
    return NULL;
}

int init_matrix()
{
    //矩阵初始化
    matrix = (int **)malloc(N * sizeof(int *));
    for(int i=0;i<N;i++)
    {
        matrix[i] = (int *)malloc(N * sizeof(int));
    }
    int cnt = 0;
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            matrix[i][j] = cnt;
            cnt++;
        }
    }
    return 0;
}

int main()
{

    int i, j;
    for (i = 0; i < N_Num; i++)
    {
        printf("直角划分方法实现矩阵转置（N=%d)\n", NN[i]);
        printf("N\t\t\tThread_Num\t\tCost Time(ms)\t\tSpeed_up\t\tEffciency(in %)\n");
        N = NN[i];
        
        //初始化矩阵
        init_matrix();

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
                    pthread_create(&threads[pthread_no], NULL, right_angle_division, &x[pthread_no]);

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

            //打印相关数据
            printf("%d\t\t\t", N);
            printf("%d\t\t\t", Thread_Num);
            printf("%.0f\t\t\t", current_time);
            printf("%.5f\t\t\t", speed_up);
            printf("%.3f\n", effciency);

            
        }
        free(matrix);

        printf("\n");
    }
    return 0;
}

