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


//输入输出矩阵，传递时候的临时变量 
int **matrix, **out, *tmp;
int n;
double t_start, t_end,current_time;
MPI_Status status;
int Process_Num;

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

	//进程数开根号即每行或每列的子块数，列号，行号，每个块的维数
	int sqrt_group_size,u,v,length;
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
	sqrt_group_size = sqrt(group_size);
	u = my_rank%sqrt_group_size;
	v = my_rank/sqrt_group_size;
	length = n/sqrt_group_size;
	tmp = (int *)malloc(sizeof(int)*length*length);

	if(my_rank == 0)
	{
		// 初始化矩阵
		init();
		t_start = MPI_Wtime();
		
   		//这两层循环是找到对应编号的子块
		for(int i=0;i<sqrt_group_size;i++)
		{
			for(int j=0;j<sqrt_group_size;j++)
			{
				//确定每一个子块中的元素对应在matrix里的位置了 
				//传递的数据需存在一维数组中，二维可能存在内存不连续的状况
				int point = 0;
				for(int k=length*i;k<length*(i+1);k++)
				{
					for(int l=length*j;l<length*(j+1);l++)
					{
						tmp[point] = matrix[k][l];
						point++;
					}
				}

				//0号子块由0号进程自己转置
				if(i==0 && j==0)
				{
					for(int i=0;i<length;i++)
					{
			            for(int j=0;j<length;j++)
						{
			                out[i][j] = matrix[j][i];
			            }
			        }
				}
				//其他子块送到其他进程中去转置 
				else{
					MPI_Send(tmp,length*length,MPI_INT,i*sqrt_group_size+j,i*sqrt_group_size+j,MPI_COMM_WORLD);
				}
			}
		}

    }
    else
	{
    	//printf("myrank=%d\n",my_rank);
    	MPI_Recv(tmp,length*length,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);
    	//内部转置 
    	int t;
        for(int i=0;i<length;i++)
		{
            for(int j=i+1;j<length;j++)
			{
                t = tmp[i*length+j];
                tmp[i*length+j] = tmp[j*length+i];
                tmp[j*length+i] = t;
            }
        }
        MPI_Send(tmp,length*length,MPI_INT,0,my_rank,MPI_COMM_WORLD);
    }
    
    if(my_rank==0)
	{
    	//还是先找每一个子块对应的进程 
        for(int i=0;i<sqrt_group_size;i++)
		{
            for(int j=0;j<sqrt_group_size;j++)
			{
            	if(i!=0 || j!=0){
            		//printf("0 receive myrank=%d\n",i*sqrt_group_size+j);
	                MPI_Recv(tmp,length*length,MPI_INT,i*sqrt_group_size+j,i*sqrt_group_size+j,MPI_COMM_WORLD,&status);
	                //printf("0 receive myrank=%d successfully\n",i*sqrt_group_size+j);
					for(int x=0;x<length;x++)
					{
	                    for(int y=0;y<length;y++)
						{
	                    	//要交换的子块进行交换 
	                        out[j*length+x][i*length+y] = tmp[x*length+y];
	                    }
	                }
            	}
            }
        }
        t_end = MPI_Wtime();

		current_time = (t_end - t_start) * 1000;

		//打印相关数据
		printf("%d\t\t", n);
		printf("%.5f\n", current_time);

        // print_matrix();
        free(matrix);
    }

    free(tmp);
	free(out);
	MPI_Finalize();
	return 0;
}

