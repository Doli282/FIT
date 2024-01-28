#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace std;

class A
 {
   public:
             A ( int x ) { m_X = x; }
    virtual ~A ( void ) { } 
    friend ostream & operator << ( ostream & os, const A & x ) 
     { return os << x . m_X; }
   private: 
    int m_X;
 };

class B : public A
 {
   public:
          B ( int x, int y ) : A ( x ) { m_Y = y; }
    friend ostream & operator << ( ostream & os, const B & x ) 
     { os << (const A &) x; return os << x . m_Y; }
   private: 
    int m_Y;
 };

void foo ( A & val )
 {
   cout << val;
 }

int main ( void )
 {
   B test ( 64, 74 );

   foo ( test );
   return 0;
 }