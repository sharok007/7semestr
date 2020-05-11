#include <string>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <pthread.h>
#include "vectorstruct.h"

template <class T>
struct pthreadArgs{
    int lowBord;
    int upperBord;
    int size;
    T **a;
    T **b;
    T **c;
};

template <class T>
void allocationMemory(T ***a, T ***b, T ***c, int size); //функция выделения памяти

template <typename T>
void fillingMatrix(T **a, T **b, int size, std::string key); //функция заполнения матрицы

void* pthreadSumMatrixInt(void* args); //функции для pthread
void* pthreadSumMatrixVec(void* args);
void* pthreadSumMatrixDoub(void* args);

template <class T>
void summa(T **a, T **b, T **c, int size, std::string method, int countProc); //функция сложения матриц

template <class T>
void printResult(T **c, int size, double duration); //функция печати результата

template <class T>
int start(std::string key, int size, std::string method, int countProc); //функция запуска

template <class T>
void freeMemory(T **a, T **b, T **c, int size); //функция освобождения памяти

int main(int argc, char *argv[])
{
    if(argc < 4){
	printf("Недостаточно аргументов!\n");
	return -1;
    }

    srand(time(0));

    std::string key = argv[1];
    int size = atoi(argv[2]);
    std::string method = argv[3];
    int countProc = atoi(argv[4]);

    if(key == "-i" || key == "--int"){
	start<int>(key, size, method, countProc);
    }

    else if(key == "-d" || key == "--double"){
	start<double>(key, size, method, countProc);
    }

    else if(key == "-v" || key == "--vector"){
	MyVector  **a, **b, **c;
	allocationMemory<MyVector>(&a, &b, &c, size);
	summa<MyVector>(a, b, c, size, method, countProc);
	freeMemory<MyVector>(a, b, c, size);
    }

    else {
	std::cout << "No correct key " << key << std::endl;
	return -1;
    }
    return 0;
}

template <class T>
void allocationMemory(T ***a, T ***b, T ***c, int size){
    *a = new T* [size];
    *b = new T* [size];
    *c = new T* [size];
    for (int i = 0; i < size; ++i){
	(*a)[i] = new T[size];
	(*b)[i] = new T[size];
	(*c)[i] = new T[size];
    }

}

template <typename T>
void fillingMatrix(T **a, T **b, int size, std::string key){

    if(key == "-i" || key == "--int"){
	for (int i = 0; i < size; ++i){
	    for (int j = 0; j < size; ++j){
		a[i][j] = (rand()%20 - 10);
		b[i][j] = (rand()%20 - 10);
	    }
	}
	return;
    }

    else if(key == "-d" || key == "--double"){
	for(int i = 0; i < size; ++i){
	    for (int j = 0; j < size; ++j){
		a[i][j] = (rand()%20 - 10)/static_cast<double>(rand()%20 + 1);
		b[i][j] = (rand()%20 - 10)/static_cast<double>(rand()%20 + 1);
	    }
	}
	return;
    }
}

void* pthreadSumMatrixInt(void* args){
    pthreadArgs <int> arg = *((pthreadArgs<int>*) args);
    for(int i = arg.lowBord; i < arg.upperBord; ++i){
	for(int j = 0; j < arg.size; ++j){
	    arg.c[i][j] = arg.a[i][j] + arg.b[i][j];
	}
    }
    return 0;
}

void* pthreadSumMatrixVec(void* args){
    pthreadArgs <MyVector> arg = *((pthreadArgs<MyVector>*) args);
    for(int i = arg.lowBord; i < arg.upperBord; ++i){
	for(int j = 0; j < arg.size; ++j){
	    arg.c[i][j] = arg.a[i][j] + arg.b[i][j];
	}
    }
    return 0;
}

void* pthreadSumMatrixDoub(void* args){
    pthreadArgs <double> arg = *((pthreadArgs<double>*) args);
    for(int i = arg.lowBord; i < arg.upperBord; ++i){
	for(int j = 0; j < arg.size; ++j){
	    arg.c[i][j] = arg.a[i][j] + arg.b[i][j];
	}
    }
    return 0;
}

template <class T>
void summa(T **a, T **b, T **c, int size, std::string method, int countProc){
    double start, end, duration;

    if(countProc > size){
	countProc = size;
    }
    if(method == "-o" || method == "--omp"){
	omp_set_num_threads(countProc);
	int i, j;
	start = omp_get_wtime();
#pragma omp parallel for shared(a, b, c) private(i, j)
	for(i = 0; i < size; ++i){
	    for(j = 0; j < size; ++j){
		c[i][j] = a[i][j] + b[i][j];
	    }
	}
#pragma omp barrier
	end = omp_get_wtime();
	duration = end - start;
    }
    else if(method == "-p" || method == "--pthread"){
	pthread_t threads[countProc];
	pthreadArgs <T> funcArgs[countProc];
	int lowBords[countProc];
	int upperBords[countProc];
	for (int i = 0; i < countProc; ++i)
	{
	    int portion = size / countProc;
	    lowBords[i] = i * portion;
	    if ((i + 1 == countProc) && (size % countProc != 0))
	    {
		upperBords[i] = size;
	    }
	    else
	    {
		upperBords[i] = lowBords[i] + portion;
	    }
	    funcArgs[i].lowBord = lowBords[i];
	    funcArgs[i].upperBord = upperBords[i];
	    funcArgs[i].a = a;
	    funcArgs[i].b = b;
	    funcArgs[i].c = c;
	    funcArgs[i].size = size;
	}


	start = omp_get_wtime();
	for (int i = 0; i < countProc; i++)
	{
	    if(typeid (T) == typeid (int)){
		pthread_create(&threads[i], NULL, pthreadSumMatrixInt, (void *)(&funcArgs[i]));
		continue;
	    }else if(typeid (T) == typeid (double)){
		pthread_create(&threads[i], NULL, pthreadSumMatrixDoub, (void *)(&funcArgs[i]));
		continue;
	    }else {
		pthread_create(&threads[i], NULL, pthreadSumMatrixVec, (void *)(&funcArgs[i]));
		continue;
	    }
	}

	for (int i = 0; i < countProc; i++)
	{
	    pthread_join(threads[i], NULL);
	}
	end = omp_get_wtime();
	duration = end - start;
    }

    printResult(c, size, duration);
}

template <class T>
void printResult(T **c, int size, double duration){
    for (int i = 0; i < size; ++i)
    {
	for (int j = 0; j < size; ++j)
	{
	    std::cout << "  " << c[i][j];
	}
	std::cout << std::endl;
    }
    std::cout << std::endl << "Время: " << duration << std::endl;
}
template <class T>
void freeMemory(T **a, T **b, T **c, int size){
    for (int i = 0; i < size; ++i){
	delete [] a[i];
	delete [] b[i];
	delete [] c[i];
    }

    delete [] a;
    delete [] b;
    delete [] c;
}

template <class T>
int start(std::string key, int size, std::string method, int countProc){
    T **a, **b, **c;

    allocationMemory<T>(&a, &b, &c, size);
    fillingMatrix<T>(a, b, size, key);
    summa<T>(a, b, c, size, method, countProc);
    freeMemory<T>(a, b, c, size);
    return 0;
}
