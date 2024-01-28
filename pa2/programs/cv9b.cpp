#include <vector>
#include <iostream>
#include <list>

using namespace std;


template<class T, class B>
T myaccumulate(B begin, B end){
    T init;
    for(auto it = begin; it != end; ++it){
        init = init + *it;
    }
    return init;
}

int main(){

    vector<int> vec = {1,2,3};
    int init = 0;
    init = myaccumulate<int, vector<int>::iterator>(vec.begin(), vec.end());
    cout << init << endl;

    list<string> vecs = {"1","2","3"};
    string inits;
    inits = myaccumulate<string, list<string>::iterator>(vecs.begin(), vecs.end());
    cout << inits << endl;

    return 0;
}