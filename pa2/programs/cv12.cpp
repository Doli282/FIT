#include <cstddef>
#include <vector>
#include <cassert>
#include <list>
#include <iostream>
#include <ctime>
#include <cstdlib>

template<typename T>
class LinkedList {
    struct Node {
        Node(T val, Node* next = nullptr) : val(val), next(next) {}
        T val;
        Node *next;
        ~Node() {
            delete next;
        }
    };
    Node* head;
    Node* tail;

    /// Copies linked list, other must not be empty list.
    Node* CopyList(Node* other) {
        assert(other != nullptr);
        if (other->next == nullptr) {
            tail = new Node(other->val, nullptr);
            return tail;
        }
        return new Node(other->val, CopyList(other->next));
    }
public:
    class Iterator {
        Node* ptr;
    public:
        Iterator(Node *node) : ptr(node) {}
        /// Returns member at which the iterator points
        T& operator*() {
            return ptr->val;
        }

        /// Advances iterator one position forward
        Iterator operator++(int) {
            Iterator it(ptr);
            ++(*this);
            return it;
        }

        Iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        /// Compares two iterators.
        friend bool operator==(const Iterator &iter1, const Iterator &iter2) {
            return iter1.ptr == iter2.ptr;
        }

        friend bool operator!=(const Iterator &iter1, const Iterator &iter2) {
            return !(iter1 == iter2);
        }
    };

    class ConstIterator {
        Node* ptr;
    public:
         ConstIterator(Node *node) : ptr(node) {}
        /// Returns member at which the iterator points
        const T& operator*() const {
            return ptr->val;
        }

        /// Advances iterator one position forward
        ConstIterator operator++(int) {
            ConstIterator it(ptr);
            ++(*this);
            return it;
        }

        ConstIterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        /// Compares two iterators.
        friend bool operator==(const ConstIterator &iter1, const ConstIterator &iter2) {
            return iter1.ptr == iter2.ptr;
        }

        friend bool operator!=(const ConstIterator &iter1, const ConstIterator &iter2) {
            return !(iter1 == iter2);
        }
    };

    LinkedList() : head(nullptr), tail(nullptr) {
        std::cout << "default constructor" << std::endl;
    }

    LinkedList(const LinkedList &other) {
        if (other.head == nullptr) {
            head = tail = nullptr;
        }
        std::cout << "copy constructor" << std::endl;
		head = CopyList(other.head);
    }

    LinkedList(LinkedList&& other) {
        head = other.head;
        tail = other.tail;
        other.head = other.tail = nullptr;
        std::cout << "move constructor" << std::endl;
    }

    LinkedList& operator=(LinkedList other) {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::cout << "move =" << std::endl;
        return *this;
    }

    ~LinkedList() {
        delete head;
    }

    void append(T val)
    {
        Node* new_node = new Node(val);
        if (head == nullptr) {
            head = tail = new_node;
            return;
        }
        tail->next = new_node;
        tail = new_node;
    }

    void prepend(T val)
    {
        Node* new_node = new Node(val);
        if (head == nullptr) {
            head = tail = new_node;
            return;
        }
        new_node->next = head;
        head = new_node;
    }

    /// Iterator to first element of the list
    Iterator begin() {
        return Iterator(head);
    }

    ConstIterator begin() const {
        return ConstIterator(head);
    }

    /// Iterator to first element of the list
    ConstIterator cbegin() const {
        return ConstIterator(head);
    }

    /// Iterator to 'past' the end element of the list
    Iterator end() {
        return Iterator(nullptr);
    }

    ConstIterator end() const {
        return ConstIterator(nullptr);
    }

    /// Iterator to 'past' the end element of the list
    ConstIterator cend() const {
        return ConstIterator(nullptr);
    }

    void print(std::ostream& os) const {
        for (auto it = cbegin(); it != cend(); ++ it) {
            os << *it << " ";
        }
        os << "\n";
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const LinkedList<T>& l) {
    for (const auto &x : l) {
        os << x << " ";
    }
    return os;
}

LinkedList<std::string> generate(size_t lst_size, size_t str_size) {
    const char table[] = "abcdefghijklmnopqrstuvwxyz";
    LinkedList<std::string> result;
    while (lst_size--) {
        std::string s;
        for (size_t i = 0; i < str_size; ++ i) {
            s.push_back(table[rand() % (sizeof(table) - 1)]);
        }
        result.append(s);
    }
    return result;
}

int main() {
    LinkedList<std::string> l = std::move(generate(10, 5));
    std::cout << l << std::endl;
}
