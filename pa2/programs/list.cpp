#include <vector>
#include <cassert>
#include <ctime>
#include <cstdlib>

class LinkedList {
    // TODO: private members
public:
    struct Element{
        int value;
        Element * next= nullptr;
    };
    Element * head = nullptr;
    class Iterator {
    public:
        /// Returns member at which the iterator points
        int& operator*(){
            return pElement->value;
        }

        /// Advances iterator one position forward
        Iterator operator++(int){
            Iterator tmp = *this;
            if(pElement->next == nullptr){
                pElement = head;
            } else {
                pElement = pElement->next;
            }
            return tmp;
        }
        Iterator& operator++(){
            if(pElement->next == nullptr){
                pElement = head;
            } else {
                pElement = pElement->next;
            }
            return *this;
        }

        /// Compares two iterators.
        friend bool operator==(const Iterator &iter1, const Iterator &iter2);
        friend bool operator!=(const Iterator &iter1, const Iterator &iter2);
        
        
        Element * pElement = nullptr;
    };
    class ConstIterator {
    public:
        /// Returns member at which the iterator points
        const int& operator*() const;

        /// Advances iterator one position forward
        ConstIterator operator++(int);
        ConstIterator& operator++();

        /// Compares two iterators forward
        friend bool operator==(const Iterator &iter1, const Iterator &iter2);
        friend bool operator!=(const Iterator &iter1, const Iterator &iter2);
    };

    void append(int val);
    void prepend(int val);

    /// Iterator to first element of the list
    Iterator begin() const;

    /// ConstIterator to first element of the list
    ConstIterator begin() const;

    /// Iterator to 'past' the end element of the list
    Iterator end() const;
    
    LinkedList(const LinkedList &other);

    LinkedList& operator=(const LinkedList &other);

    friend void swap(LinkedList &l1, LinkedList &l2);

    // If you're bored
    /// Inserts new value before 'it' and returns iterator to it.
    Iterator insert(Iterator it, int val);
    /// Erases value at given iterator and returns 
    /// iterator to value following the erased one. 
    Iterator erase(Iterator it);
};

bool operator==(const Iterator &iter1, const Iterator &iter2){
    return (iter1->pElement == iter2->pElement);
}
bool operator!=(const Iterator &iter1, const Iterator &iter2){
    return (iter1->pElement != iter2->pElement);
}

int main() {
    // TODO: Tests
}
