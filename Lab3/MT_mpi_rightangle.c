#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <assert.h>
#include <fstream>
#include <iostream>
using namespace std;

#define max 10000

//输入输出矩阵，传递时候的临时变量 
int **matrix, **out, *temp;
int n;
double t_start, t_end,current_time;
MPI_Status status;
double serial_time;
double speed_up;
double effciency;
int Process_Num;
static double serial_time_list[max] = {0.0};

__attribute__((constructor)) void load_file()
{
	//这部分函数不影响MPI_Wtime的计时，用来测试在这个函数里花了多少时间
	// struct timeval time_start,time_end;
	// gettimeofday(&time_start, NULL);

	const char* file = "./MT_mpi_rightangle_process_1.txt";
    // printf("have entered find_serial_time\n");

	ifstream infile;
	int N;
	double cost_time;
	infile.open(file);
	assert(infile.is_open());

	while(infile >>N >> cost_time)
	{
		// printf("N is %d, cost_time is %.5f\n",N,cost_time);
		serial_time_list[N] = cost_time;
	}
	infile.close();
	
	// gettimeofday(&time_end, NULL);
    // float timetime = (float)((1000000 * (time_end.tv_sec - time_start.tv_sec) + time_end.tv_usec - time_start.tv_usec) / 1000);
	// printf("The time spend in before main() is %0.2f\n", timetime);

	return ;
} 

//初始化矩阵
void init()
{
	matrix = (int **)malloc(n * sizeof(int *));
	out = (int **)malloc(n * sizeof(int *));
    for(int i=0;i<n;i++)
	{
        matrix[i] = (int *)malloc(n * sizeof(int));
        out[i] = (int *)malloc(n * sizeof(int));
    }
	int cnt = 0;
    for(int i=0;i<n;i++)
	{
    	for(int j=0;j<n;j++)
		{
    		matrix[i][j] = out[i][j] = cnt;
    		cnt++;
		}
	}
}

//打印矩阵
void print_matrix()
{
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			printf("%d ",out[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	//矩阵维数 
	n = atoi(argv[1]);
	
	// printf("entered main()\n");
	// //寻找串行时间
	serial_time = serial_time_list[n];

	//进程数开根号即每行或每列的子块数，列号，行号，每个块的维数
	int sqrt_group_size,x,y,length;
	int my_rank,group_size;

	//MPI部分
	//init之前不是简单的想象中并行，这里对于每个进程而言都是串行执行了一下 
	//开始并行程序 
	MPI_Init(&argc, &argv);
	//获取进程数 
	MPI_Comm_size(MPI_COMM_WORLD,&group_size);
	//获取PID 
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	//根据当前可使用的进程数（group_size），初始化相应变量
	Process_Num = group_size;
	sqrt_group_size = n / group_size;
	x = my_rank * sqrt_group_size;
	y = (my_rank == group_size -1) ? n : x+sqrt_group_size;
	temp = (int *)malloc(sizeof(int)*n*n);

	if(my_rank == 0)
	{
        // 初始化矩阵
	    init();
	    t_start = MPI_Wtime();

		for(int i=0;i<group_size;i++)
		{
			if(i == 0)
			{
				for(int j=x;j<y;j++)
					for(int k=0; k<=j; k++)
					{
						out[k][j] = matrix[j][k];
						out[j][k] = matrix[k][j];
					}
			}
			else
			{
				int xx = i * sqrt_group_size;
				int yy = (i == group_size - 1) ? n : xx+sqrt_group_size;

				for(int j =xx;j<yy;j++)
					for(int k=0;k<=j;k++)
					{
						temp[j*n+k] = out[j][k];
						temp[k*n+j] = out[k][j];
						// cout<<temp[j*n+k]<<" "<<temp[k*n+j]<<endl;
					}
				MPI_Send(temp,n*n,MPI_INT,i,i,MPI_COMM_WORLD);
			}
		}
	}
	else
	{
		MPI_Recv(temp,n*n,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);
		int swp;
		for(int j=x;j<y;j++)
			for(int k=0;k<=j;k++)
			{
				swp = temp[j*n+k];
				temp[j*n+k] = temp[k*n+j];
				temp[k*n+j] = swp;
			}
		MPI_Send(temp,n*n,MPI_INT,0,my_rank+n,MPI_COMM_WORLD);
	}
	
	if(my_rank == 0){
        for(int i=1;i<group_size;i++){
            
                MPI_Recv(temp,n*n,MPI_INT,i,i+n,MPI_COMM_WORLD,&status);
                int x = i * sqrt_group_size;
                int y = (i == group_size-1) ? n : x+sqrt_group_size;
                for(int j=x;j<y;j++)
                    for(int k=0;k<=j;k++)
					{
                        out[j][k] = temp[j*n+k];
                        out[k][j] = temp[k*n+j];
                    }
            
        }

        t_end = MPI_Wtime();

		current_time = (t_end - t_start) * 1000;
		speed_up = serial_time / current_time;
        effciency = speed_up / Process_Num * 100;

		//打印相关数据
		printf("%d\t\t", n);
		printf("%d\t\t", Process_Num);
		printf("%.5f\t\t", current_time);
		printf("%.5f\t\t", speed_up);
		printf("%.3f\n", effciency);

        // print_matrix();
        free(matrix);
    }

    free(temp);
	free(out);
	MPI_Finalize();
	return 0;
}