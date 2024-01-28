//#include <stdio.h>
//#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

//typedef struct array { /***************/
struct array_t {
	size_t size;
	int * data;
//} array_t;
};

/*void createArray(array_t* array, int array_size) { 
	size_t i;
	array->size = array_size;
	array->data = (int*) malloc(array->size * sizeof(int)); 
	for (i = 0; i < array->size; ++i) {
		array->data[i] = i;
	}
}
*/
void createArray(array_t & array, int array_size){
    array.size = array_size;
    array.data = new int [array.size];
    for(size_t i = 0; i < array.size; ++i){
        array.data[i] = i;
    }
}

/*void printArray(const array_t* array) { 
	size_t i;

	for (i = 0; i < array->size; ++i) {
		printf("%d ", array->data[i]); 
	}
	printf("\n"); 
}*/
void printArray(array_t array){
    for(size_t i = 0; i < array.size; ++i){
        cout << array.data[i];
    }
    cout << endl;
}

//void freeArray(array_t* array){ /***************/
void freeArray(array_t & array){
	//free(array->data); /***************/
	delete[] array.data;
    delete[] array.data;
    array.data = NULL;
}


int main() {
	int array_size = 0;
	array_t array = {0, NULL};

	//printf("What is the size of an array I should create?\n"); /***************/
    cout << "What is the size of an array I should create?" << endl;
	//scanf("%d", &array_size); /***************/
    if(!(cin >> array_size)){
        cout << "chyba" << endl;
        return 1;
    }

	createArray(array, array_size);
	printArray(array);
	freeArray(array);
	
	
	return 0;
}