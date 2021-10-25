#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


//n为矩阵的长度或宽度
int N;
//n的个数
#define N_Num  8 
int NN[N_Num] = {256,512,1024,2048,4096,8192,16384,32768};

int Thread_Num = 1;

//串行计算的时间，即单线程
float time;
//矩阵
int **matrix;
//两个变量相互交换中用到的中间变量
int temp=0;

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
        printf("串行法实现矩阵转置（N=%d)\n", NN[i]);
        printf("N\t\t\tThread_Num\t\tCost Time(ms)\n");
        N = NN[i];
        
        //初始化矩阵
        init_matrix();

        //判断输入是否合理
        if (Thread_Num > N || Thread_Num <= 0 || N <= 0)
        {
            printf("Wrong input!\n");
            return 0;
        }

        //记录开始时间
        struct timeval time_start, time_end;
        gettimeofday(&time_start, NULL);

        for(int i = 0; i< N;i++)
        {
            for(int j = 0;j<i;j++)
            {
                temp = matrix[i][j];
                matrix[i][j] = matrix[j][i];
                matrix[j][i] = temp;
            }
                
        }
        
        //记录结束时间，算出线程运行时间、加速比，效率、计算出的pi值得精确度
        gettimeofday(&time_end, NULL);
        time = (float)((1000000 * (time_end.tv_sec - time_start.tv_sec) + time_end.tv_usec - time_start.tv_usec) / 1000);
        

        //打印相关数据
        printf("%d\t\t\t", N);
        printf("%d\t\t\t", Thread_Num);
        printf("%.0f\n", time);

        free(matrix);
    }
    return 0;
}

