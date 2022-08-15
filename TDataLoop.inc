#include <iostream>
#include <sstream>

// default constructor creates an empty TDataloop
template<typename T>
TDataLoop<T>::TDataLoop() : start(nullptr), count(0) { }

// non-default constructor that creates a TDataLoop with one element
template<typename T>
TDataLoop<T>::TDataLoop(const T &value) : start(nullptr), count(1) {
    start = new _Node({value, nullptr, nullptr});
    start->next = start;
    start->prev = start;
}

// copy constructor that creates a copy of the parameter TDataLoop (rhs)
template<typename T>
TDataLoop<T>::TDataLoop(const TDataLoop & rhs) {
    start = nullptr;
    count = 0;

    // uses assignment operator to update elements of new DataLoop
    *this = rhs;
}

// assignment operator that assigns a TDataLoop to another TDataLoop
template<typename T>
TDataLoop<T> & TDataLoop<T>::operator=(const TDataLoop & rhs) {

    // deallocates dynamically allocated memory in the implicit TDataLoop parameter
    clear();

    _Node *cur_node = rhs.start;

    // updates contents of implicit TDataLoop parameter
    for (size_t i = 0; i < rhs.count; i++) {
        *this += cur_node->data; // count is updated by += function
        cur_node = cur_node->next;
    }

    return *this;
}

// deallocates dynamically allocated memory in TDataLoop
template<typename T>
void TDataLoop<T>::clear() {
    _Node *cur = start;
    while (count) {
        _Node* temp = cur;
        cur = cur->next;
        delete temp;
        count--;
    }
    start = nullptr;
    count = 0;
}

// destructor that deallocates dynamically allocated memory
template<typename T>
TDataLoop<T>::~TDataLoop() {
    clear(); 
}

// compares the current TDataLoop with the input TDataLoop, returning true if they're the same node by node
template<typename T>
bool TDataLoop<T>::operator==(const TDataLoop & rhs) const {

    // returns false if counts are different
    if (count != rhs.count) {
        return false;
    }

    _Node *cur_node = start;
    _Node *rhs_node = rhs.start;

    // returns false if nodes are different
    for (size_t i = 0; i < count; i++) {
        if (cur_node->data != rhs_node->data) {
            return false;
        }
        cur_node = cur_node->next;
        rhs_node = rhs_node->next;
    }

    return true;
}

// adds a value to the end of the TDataLoop
template<typename T>
TDataLoop<T> & TDataLoop<T>::operator+=(const T & value) {

    // new node to be added to TDataLoop
    _Node *new_node = new _Node({value, nullptr, nullptr});

    // adds the new node to an empty TDataLoop
    if (count == 0 && start == nullptr) {
        start = new_node;
        new_node->prev = start;
        new_node->next = start;
        count++;
    }
    else {
        // sets cur_node to last node in the TDataLoop
        _Node *cur_node = start;
        while (cur_node->next != start) {
            cur_node = cur_node->next;
        }

        // adds new node to the TDataloop
        new_node->prev = cur_node;
        new_node->next = start;

        // updates pointers to adjacent nodes of second-to-last node and first node
        cur_node->next = new_node;
        start->prev = new_node;
        count++;
    }

    return *this;
}

// creates a third TDataLoop by concatenating copies of the current TDataLoop and the new DataLoop
template<typename T>
TDataLoop<T> TDataLoop<T>::operator+(const TDataLoop & rhs) const {

    TDataLoop new_data_loop = *this; // = operator

    // rhs TDataLoop contains no nodes
    if (rhs.count == 0 && rhs.start == nullptr) {
        return new_data_loop;
    }
    // current TDataLoop contains no nodes
    else if (count == 0 && start == nullptr) {
        new_data_loop = rhs;
        return new_data_loop;
    }

    _Node *cur_node = rhs.start;

    for (size_t i = 0; i < rhs.count; i++) {
        new_data_loop += cur_node->data; // += operator updates count
        cur_node = cur_node->next;
    }

    return new_data_loop;
}

// shifts the start position in *this TDataLoop according to the parameter offset
// forward for a positive value and backward for a negative value
template<typename T>
TDataLoop<T> & TDataLoop<T>::operator^(int offset) {

    // no change made to start if TDataLoop is empty, DTataLoop has one node, or the offset is 0
    if (count == 0 || count == 1 || offset == 0) {
        return *this;
    }
    
    // forward offset
    if (abs(offset) == offset) {
        for (int i = 0; i < offset; i++) {
            start = start->next;
        }
    }
    // backwards offset
    else {
        for (int i = 0; i < abs(offset); i++) {
            start = start->prev;
        }
    }

    return *this;
}

// inserts the entire parameter TDataLoop (rhs) into the current TDataLoop (*this)
// at the indicated position (pos) and makes rhs an empty list
template<typename T>
TDataLoop<T> & TDataLoop<T>::splice(TDataLoop & rhs, size_t pos) {

    // rhs has no nodes
    if (rhs.count == 0) {
        rhs.clear();
        return *this;
    }
    // current TDataLoop has no nodes
    else if (count == 0) {
        *this = rhs; // uses overloaded assignment operator
        rhs.clear();
        return *this;
    }

    // inserts rhs TDataLoop into current TDataLoop and updates starts
    if (pos == 0) {
        _Node *cur_rhs_node = rhs.start;
        _Node *start_node;
        bool start_bool = true;
        
        for (size_t i = 0; i < rhs.count; i++) {
            *this += cur_rhs_node->data;
            if (start_bool) {
                start_node = start->prev;
                start_bool = false;
            }
            cur_rhs_node = cur_rhs_node->next;
        }

        start = start_node;
        rhs.clear();
    }
    // inserts rhs TDataLoop into current TDataLoop and does not update start
    else {
        _Node *insert_pos = start;
        
        for (size_t i = 0; i < pos; i++) {
            insert_pos = insert_pos->next;
        } 

        _Node *cur_rhs_node = rhs.start;
        _Node *actual_start = start;
        start = insert_pos;

        for (size_t i = 0; i < rhs.count; i++) {
            *this += cur_rhs_node->data;
            cur_rhs_node = cur_rhs_node->next;
        }
        
        start = actual_start;
        rhs.clear();
    }

    return *this; 
}

// outputs the value of each node in the TDataLoop
template<typename T>
std::ostream & operator<<(std::ostream & os, const TDataLoop<T> & dl) {
    if (dl.count == 0) {
        os << ">no values<";
    }
    else {
        typename TDataLoop<T>::_Node *cur_node = dl.start;
        os << "-> ";
        for (size_t i = 0; i < dl.count; i++) {
            if (i == dl.count - 1) {
                os << cur_node->data << " <-";
            }
            else {
                os << cur_node->data << " <--> ";
            }
            cur_node = cur_node->next;
        }
    }
    return os;
}