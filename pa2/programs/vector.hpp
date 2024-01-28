#include <cstdlib>
#include <iostream>

class Vector {
    int * data;
    size_t capacity;
    size_t cnt;

public:
    Vector(size_t len);
    ~Vector();
    size_t size() const;
    void push_back(int val);
    void pop_back();
    int back();
    //void clear();
    int& at(size_t pos);
};