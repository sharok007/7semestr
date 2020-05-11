#include <string>
#include <cstdlib>
#include <ctime>

#include "vectorstruct.h"

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

template <class T>
void summa(T **a, T **b, T **c, int size){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			c[i][j] = a[i][j] + b[i][j];
		}
	}
}

template <class T>
void printResult(T **c, int size){
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			std::cout << c[i][j] << " ";
		}
		std::cout << std::endl;
	}
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
int start(std::string key, int size){
	T **a, **b, **c;

	allocationMemory<T>(&a, &b, &c, size);
	fillingMatrix<T>(a, b, size, key);		
	summa<T>(a, b, c, size);
	printResult<T>(c, size);
	freeMemory<T>(a, b, c, size);
	return 0;
}

int main(){
	srand(time(0));

	int size;
	std::string key;
	std::cout << "enter size: ";
	std::cin >> size;
	std::cout << "enter key: ";
	std::cin >> key;
	
	if(key == "-i" || key == "--int"){
		start<int>(key, size);
	}

	else if(key == "-d" || key == "--double"){
		start<double>(key, size);
	}

	else if(key == "-v" || key == "--vector"){
		MyVector  **a, **b, **c;
		allocationMemory<MyVector>(&a, &b, &c, size);
		summa<MyVector>(a, b, c, size);
		printResult<MyVector>(c, size);
		freeMemory<MyVector>(a, b, c, size);
	}

	else {
		std::cout << "No correct key " << key << std::endl;
		return -1; 
	}
	return 0;
}