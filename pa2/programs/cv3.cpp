#include "vector.hpp"
// unit8_t - byte

int main(){
    Vector v(3);
    v.push_back(1);
    v.push_back(2);
    v.push_back(8);
    v.push_back(2);
    v.pop_back();
    std::cout << v.back() << std::endl;
}