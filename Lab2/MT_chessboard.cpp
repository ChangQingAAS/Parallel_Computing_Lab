#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include<iomanip>
using namespace std;

//n为矩阵的长度或宽度
int N;
//n的个数
#define N_Num  12
int NN[N_Num] = {16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
// # define N_Num 1
// int NN[N_Num] = {8};

int Thread_Num;
//线程数的个数
#define TN_Num 5   
int TN[TN_Num] = {1,4,16,64,256};
// # define TN_Num 1
// int TN[TN_Num] = {16};

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
//子块数目
int p;
//根号p
int sqrt_p;
//子块长度或宽度
int block_length;
//两个变量相互交换中用到的中间变量
int temp=0;
// 标志每个块是否被转置过
int **flag;
        
int print_matrix()
{
    for(int i = 0; i< N;i++)
    {
        for(int j = 0;j<N;j++)
        {
            cout<<setw(5)<<matrix[i][j]<<"   ";
        }
        cout<<endl;
    }

    return 0;
}

void *chessboard_division(void *arg)
{
    int rank = *(int *)arg;
    //子块在块数组中的横坐标
    int u = rank / sqrt((double)Thread_Num);    
    //子块在块数组中的纵坐标
    int v= rank % (int)sqrt((double)Thread_Num);

    //块间转置
    if(u == v && flag[u][v] == 0)
        flag[v][v] = 1;
    else if(u != v && flag [u][v] == 0)
    {
        for(int x_step=0;x_step<block_length;x_step++)
        {
            for(int y_step = 0;y_step<block_length;y_step++)
            {
                temp = matrix[u*block_length+ x_step][v*block_length+y_step];
                matrix[u*block_length+ x_step][v*block_length+y_step] = matrix[v*block_length+ x_step][u*block_length+y_step];
                matrix[v*block_length+ x_step][u*block_length+y_step] = temp;
                // cout<<"当前交换的元素为："<<"("<<i*block_length+ x_step<<","<<j*block_length+y_step<<")和"<<"("<<j*block_length+ x_step<<","<<i*block_length+y_step<<")"<<endl;
            }
        }
        flag[u][v] = flag[v][u] = 1;
    }
    // cout<<"对矩阵块"<<i<<j<<"和矩阵"<<j<<i<<"的交换已完成"<<endl;
    // cout<<"块间转置后的矩阵为："<<endl;
    // print_matrix();
            
    //块内转置
    // cout<<"现在正在转置矩阵块("<<u<<","<<v<<")"<<endl;
    for(int x_step=1;x_step<block_length;x_step++)
    {
        for(int y_step = 0;y_step<x_step;y_step++)
        {
            temp = matrix[u*block_length+ x_step][v*block_length+y_step];
            matrix[u*block_length+ x_step][v*block_length+y_step] = matrix[u*block_length+ y_step][v*block_length+x_step];
            matrix[u*block_length+ y_step][v*block_length+x_step] = temp;
            // cout<<"当前交换的元素为："<<"("<<u*block_length+ x_step<<","<<v*block_length+y_step<<")和"<<"("<<u*block_length+ y_step<<","<<v*block_length+x_step<<")"<<endl;
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

int init_flag()
{
    flag = (int **)malloc((int)(sqrt(Thread_Num)) * sizeof(int *));
    for(int i=0;i<((int)sqrt(Thread_Num));i++)
    {
        flag[i] = (int *)malloc(((int)sqrt(Thread_Num)) * sizeof(int));
    }
    for(int i=0;i<((int)sqrt(Thread_Num));i++)
    {
        for(int j=0;j<((int)sqrt(Thread_Num));j++)
        {
            flag[i][j] = 0;
        }
    }
    return 0;
}

int main()
{

    int i, j;
    for (i = 0; i < N_Num; i++)
    {
        printf("块棋盘划分方法实现矩阵转置（N=%d)\n", NN[i]);
        printf("N\t\t\tThread_Num\t\tCost Time(ms)\t\tSpeed_up\t\tEffciency(in %)\n");
        N = NN[i];

        init_matrix();        
        // cout<<"初始矩阵为："<<endl;
        // print_matrix();

        for (j = 0; j < TN_Num; j++)
        {
            Thread_Num = TN[j];

            // 进行矩阵的块的转置
            block_length = (int)sqrt((double)(N*N/Thread_Num));
            sqrt_p = N / block_length ;
            p = N * N /(block_length*block_length);

            //初始化各个块没有被转置过
            init_flag();

            //判断输入是否合理
            if (Thread_Num <= 0 || N <= 0 || Thread_Num > N * N)
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
                    pthread_create(&threads[pthread_no], NULL, chessboard_division, &x[pthread_no]);

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

            // cout<<"块内转置后的矩阵为："<<endl;
            // print_matrix();

            //释放资源
            free(flag);

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
        //释放资源
        free(matrix);

        printf("\n");
    }
    return 0;
}

