#include <cstddef>
#include <vector>
#include <cassert>
#include <list>
#include <iostream>
#include <ctime>
#include <cstdlib>

template<typename S>
class LinkedList {
    
    template <class T>
    struct Node {
        Node(T val, Node<T> * next = nullptr) : val(val), next(next) {}
        T val;
        Node<T> *next;
        ~Node() {
            delete next;
        }
    };
    Node<S>* head;
    Node<S>* tail;

    /// Copies linked list, other must not be empty list.
    Node<S>* CopyList(Node<S>* other) {
        assert(other != nullptr);
        if (other->next == nullptr) {
            tail = new Node<S>(other->val, nullptr);
            return tail;
        }
        return new Node<S>(other->val, CopyList(other->next));
    }
public:
    template<class U>
    class Iterator {
        Node<U>* ptr;
    public:
        Iterator(Node<U> *node) : ptr(node) {}
        /// Returns member at which the iterator points
        int& operator*() {
            return ptr->val;
        }

        /// Advances iterator one position forward
        Iterator<U> operator++(int) {
            Iterator<U> it(ptr);
            ++(*this);
            return it;
        }

        Iterator<U>& operator++() {
            ptr = ptr->next;
            return *this;
        }

        /// Compares two iterators.
        friend bool operator==(const Iterator<U> &iter1, const Iterator<U> &iter2) {
            return iter1.ptr == iter2.ptr;
        }

        friend bool operator!=(const Iterator<U> &iter1, const Iterator<U> &iter2) {
            return !(iter1 == iter2);
        }
    };

    template<class V>
    class ConstIterator {
        Node<V>* ptr;
    public:
         ConstIterator(Node<V> *node) : ptr(node) {}
        /// Returns member at which the iterator points
        const int& operator*() const {
            return ptr->val;
        }

        /// Advances iterator one position forward
        ConstIterator<V> operator++(int) {
            ConstIterator<V> it(ptr);
            ++(*this);
            return it;
        }

        ConstIterator<V>& operator++() {
            ptr = ptr->next;
            return *this;
        }

        /// Compares two iterators.
        friend bool operator==(const ConstIterator<V> &iter1, const ConstIterator<V> &iter2) {
            return iter1.ptr == iter2.ptr;
        }

        friend bool operator!=(const ConstIterator<V> &iter1, const ConstIterator<V> &iter2) {
            return !(iter1 == iter2);
        }
    };

    LinkedList() : head(nullptr), tail(nullptr) {}

    void append(S val);

    void prepend(S val);

    /// Iterator to first element of the list
    Iterator<S> begin() {
        return Iterator<S>(head);
    }

    ConstIterator<S> begin() const {
        return ConstIterator<S>(head);
    }

    /// Iterator to first element of the list
    ConstIterator<S> cbegin() const {
        return ConstIterator<S>(head);
    }

    /// Iterator to 'past' the end element of the list
    Iterator<S> end() {
        return Iterator<S>(nullptr);
    }

    ConstIterator<S> end() const {
        return ConstIterator<S>(nullptr);
    }

    /// Iterator to 'past' the end element of the list
    ConstIterator<S> cend() const {
        return ConstIterator<S>(nullptr);
    }

    LinkedList(const LinkedList<S> &other) {
        if (other.head == nullptr) {
            head = tail = nullptr;
        }

		head = CopyList(other.head); // tail is set as a side effect of CopyList. How to perform better?
    }

    LinkedList& operator=(LinkedList other) {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        return *this;
    }

    ~LinkedList() {
        delete head;
    }

    void print(std::ostream& os) const {
        for (auto it = cbegin(); it != cend(); ++ it) {
            os << *it << " ";
        }
        os << "\n";
    }
};

template<typename S>
std::ostream& operator<<(std::ostream& os, const LinkedList<S>& l) {
    for (const auto &x : l) {
        os << x << " ";
    }
    return os;
}

template<typename S>
void LinkedList<S>::append(S val)
{
    Node<S>* new_node = new Node<S>(val);
    if (head == nullptr) {
        head = tail = new_node;
        return;
    }
    tail->next = new_node;
    tail = new_node;
}

template<typename S>
void LinkedList<S>::prepend(S val)
{
    Node<S>* new_node = new Node<S>(val);
    if (head == nullptr) {
        head = tail = new_node;
        return;
    }
    new_node->next = head;
    head = new_node;
}


/*
void randomTest(int op) {
    std::list<int> ref_l;
    LinkedList l;
    while(op --) {
        int val = rand() % 10000;
        if(rand() % 2) {
            ref_l.push_back(val);
            l.append(val);
        } else {
            ref_l.insert(ref_l.begin(), val);
            l.prepend(val);
        }
    }
    auto ref_it = ref_l.begin();
    auto it = l.begin();

    while(ref_it != ref_l.end()) {
        assert(*ref_it++ == *it++);
    }
    assert(it == l.end());
}*/

int main() {
    srand(time(NULL));

   // randomTest(100);
   // randomTest(1000);
   // randomTest(10000);
}
