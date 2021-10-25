#!/bin/bash
echo "已经进入该脚本"

echo "直角划分法"
echo "计算单进程的情况"

echo "mpiicpc -o MT_mpi_rightangle_process_1 MT_mpi_rightangle_process_1.c"
eval "mpiicpc -o MT_mpi_rightangle_process_1 MT_mpi_rightangle_process_1.c"

echo "rm MT_mpi_rightangle_process_1.txt"
eval "rm MT_mpi_rightangle_process_1.txt"

NN=(16 32 64 128 256 512);
len_NN=${#NN[*]};
for (( i=0; i<"$len_NN"; i=i+1 ))
do
    N=${NN[$i]}
    echo "N is $N"

    echo "mpirun -np 1 /public/home/BG083/MT_mpi_rightangle_process_1  $N >> MT_mpi_rightangle_process_1.txt"
    eval "mpirun -np 1 /public/home/BG083/MT_mpi_rightangle_process_1   $N >> MT_mpi_rightangle_process_1.txt"

done


echo "进入多进程部分"
echo "mpiicpc -o MT_mpi_rightangle MT_mpi_rightangle.c"
eval "mpiicpc -o MT_mpi_rightangle MT_mpi_rightangle.c"

echo "rm MT_mpi_rightangle.txt"
eval "rm MT_mpi_rightangle.txt"

PN=(4  16  64  256);
len_PN=${#PN[*]};

N=0;
process_number=0;

eval "echo 'N           Process_Num     Cost Time(ms)       Speed_up        Effciency(in %)' > MT_mpi_rightangle.txt "


for (( i=0; i<"$len_NN"; i=i+1 ))
do
    # echo ${NN[$i]}
    N=${NN[$i]}
    # echo $N
    for (( j=0; j<"$len_PN"; j=j+1 ))
    do
        process_number=${PN[$j]}

        # echo "process_number is $process_number"
        # echo "N is $N"

        echo "mpirun -np $process_number /public/home/BG083/MT_mpi_rightangle  $N >> MT_mpi_rightangle.txt"
        eval "mpirun -np $process_number /public/home/BG083/MT_mpi_rightangle  $N >> MT_mpi_rightangle.txt"
    done
    echo " " >> MT_mpi_rightangle.txt
    echo " " >> MT_mpi_rightangle.txt
done

echo "块棋盘法"
echo "计算单进程的情况"

echo "mpiicpc -o MT_mpi_chessboard_process_1 MT_mpi_chessboard_process_1.c"
eval "mpiicpc -o MT_mpi_chessboard_process_1 MT_mpi_chessboard_process_1.c"

echo "rm MT_mpi_chessboard_process_1.txt"
eval "rm MT_mpi_chessboard_process_1.txt"

NN=(16 32 64 128 256 512 1024 2048 4096);
len_NN=${#NN[*]};
for (( i=0; i<"$len_NN"; i=i+1 ))
do
    N=${NN[$i]}
    echo "N is $N"

    echo "mpirun -np 1 /public/home/BG083/MT_mpi_chessboard_process_1  $N >> MT_mpi_chessboard_process_1.txt"
    eval "mpirun -np 1 /public/home/BG083/MT_mpi_chessboard_process_1   $N >> MT_mpi_chessboard_process_1.txt"

done


echo "进入多进程部分"
echo "mpiicpc -o MT_mpi_chessboard MT_mpi_chessboard.c"
eval "mpiicpc -o MT_mpi_chessboard MT_mpi_chessboard.c"

echo "rm MT_mpi_chessboard.txt"
eval "rm MT_mpi_chessboard.txt"

PN=(4 9 16 25 36 49 64 81 100 121  144 169 196 225 256);
len_PN=${#PN[*]};

N=0;
process_number=0;

eval "echo 'N           Process_Num     Cost Time(ms)       Speed_up        Effciency(in %)' > MT_mpi_chessboard.txt "


for (( i=0; i<"$len_NN"; i=i+1 ))
do
    # echo ${NN[$i]}
    N=${NN[$i]}
    # echo $N
    for (( j=0; j<"$len_PN"; j=j+1 ))
    do
        process_number=${PN[$j]}

        # echo "process_number is $process_number"
        # echo "N is $N"

        echo "mpirun -np $process_number /public/home/BG083/MT_mpi_chessboard  $N >> MT_mpi_chessboard.txt"
        eval "mpirun -np $process_number /public/home/BG083/MT_mpi_chessboard  $N >> MT_mpi_chessboard.txt"
    done
    echo " " >> MT_mpi_chessboard.txt
    echo " " >> MT_mpi_chessboard.txt
done