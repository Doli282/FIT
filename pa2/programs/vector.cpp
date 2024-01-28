#include "vector.hpp"

Vector::Vector(size_t len){
    data = new int [len];
    capacity = len;
    cnt = 0;
}
Vector::~Vector(){
    delete[] data;
}
size_t Vector::size() const{
    return cnt;
}
void Vector::push_back(int val){
    if(cnt >= capacity){
        /*std::cout << "Vector is already full" << std::endl;
        return;*/
        capacity *=2;
        int * tmp = new int [capacity];
        for(size_t i = 0; i < cnt; i++){
            tmp[i] = data[i];
        }
        delete[] data;
        data = tmp;
    }
    data[cnt++] = val;
}
void Vector::pop_back(){
    if(cnt == 0){
        /*std::cout << "Vector is already empty" << std::endl;*/
        return;
    }
    cnt--;
}
int Vector::back(){
    if(cnt == 0){
        /*std::cout << "Vector is already empty" << std::endl;*/
        return 0;
    }
    return data[--cnt];
}

int & Vector::at(size_t pos){
    if(pos < capacity){
        return data[pos];
    }
    return data[cnt];
}