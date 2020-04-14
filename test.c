#include<stdio.h>
#include<mpi.h>
#include<time.h>

#define M 4
#define tegD 1
#define EL(x) (sizeof(x) / sizeof(x[0]))

double MA[M][2*M], V[2*M], MAD, D[2*M];

int main(int args, char **argv)
{
    int size, MyP, i, j, v, k, d, p;
    MPI_Status status;
    double rt, t1,t2;
    int reord = 1;
    int N; //количество строк в процессе.
    MPI_Init(&args, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyP);
    N = M/size; //Для корректной работы программы
                //количество процессов должно быть кратно размеру матрицы(например: 1,2,4)

    /* Каждая ветвь генерирует свою полосу матрицы MA и свой отрезок
 * вектора правой части, который присоединяется дополнительными
 * столбцами к MA.*/
    for(i = 0; i < N; i++)
    {
        for(j = 0; j < M; j++)
        {
            if((N*MyP+i) == j)
                MA[i][j] = 2.0;
            else
                MA[i][j] = 1.0;
        }
        for(j = M; j < 2*M; ++j)
            if((M + N*MyP+i) == j)
                MA[i][j] = 1.0;
            else
                MA[i][j] = 0.0;
    }
    for(p = 0; p < size; ++p){
        if(MyP == 0){
            printf("Source matrix:\n");
        }
        if(MyP == p){
            for(i = 0; i < N; ++i){
                for(j = 0; j < M; ++j){
                    printf("%.3f\t", MA[i][j]);
                }
                printf("\n");
            }
        }
        if(MyP == 0)
            printf("\n");
    }


    /* Каждая ветвь засекает начало вычислений и производит
 вычисления */
    MPI_Barrier(MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    /* Прямой ход */

    for(p = 0; p < size; p++)
    {
        for(k = 0; k < N; k++)
        {
            if(MyP == p)
            {
                MAD = 1.0/MA[k][N*p+k];
                for(j = 2*M-1; j >= N*p+k; j--)
                    MA[k][j] = MA[k][j] * MAD;
                for(d = p+1; d < size; d++)
                    MPI_Send(&MA[k][0], 2*M, MPI_DOUBLE, d, tegD, MPI_COMM_WORLD);
                for(i = k+1; i < N; i++)
                {
                    for(j = 2*M-1; j >= N*p+k; j--)
                        MA[i][j] = MA[i][j]-MA[i][N*p+k]*MA[k][j];
                }
            }
            else if(MyP > p)
            {
                MPI_Recv(V, EL(V), MPI_DOUBLE, p, tegD, MPI_COMM_WORLD,&status);
                for(i = 0; i < N; i++)
                {
                    for(j = 2*M-1; j >= N*p+k; j--){
                        MA[i][j] = MA[i][j]-MA[i][N*p+k]*V[j];
                    }
                }
            }
        } /* for k */
    } /* for p */

    /* Обратный ход */
    for(p = size-1; p >= 0; p--)
    {
        for(k = N-1; k >= 0; k--)
        {
            if(MyP == p)
            {
                for(i = k; i >= 0; i--){
                    for(j = N*p+k; j < 2*M; ++j){
                        MA[i][j] = MA[i][j] - MA[i][N*p+k]*MA[k][j];
                    }
                }
                for(d = p-1; d >= 0; d--)
                    MPI_Send(&MA[k], 2*M, MPI_DOUBLE, d, tegD, MPI_COMM_WORLD);
            }
            else
            {
                if(MyP < p)
                {
                    MPI_Recv(&D, EL(D), MPI_DOUBLE, p, tegD, MPI_COMM_WORLD, &status);
                    for(i = N-1; i >= 0; i--){
                        for(j = N*p+k; j < 2*M; ++j){
                            MA[i][j] -= D[j]*MA[i][N*p+k];
                        }
                    }
                }

            }
        } /* for k */
    } /* for p */

    t2 = MPI_Wtime();
    rt = t2 - t1;

    for(p = 0; p < size; ++p){
        if(MyP == 0){
            printf("Inverse matrix:\n");
        }
        if(MyP == p){
            for(i = 0; i < N; ++i){
                for(j = M; j < 2*M; ++j){
                    printf("%.3f\t", MA[i][j]);
                }
                printf("\n");
            }
        }
        if(MyP == 0)
            printf("\n");
    }


    printf("Process = %d Time = %f\n", MyP, rt);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return(0);
}
