#include "DataLoop.h"
#include <iostream>
#include <sstream>

// default constructor creates an empty Dataloop
DataLoop::DataLoop() : start(nullptr), count(0) { }

// non-default constructor that creates a dataloop with one element
DataLoop::DataLoop(const int &value) : start(nullptr), count(1) {
    start = new _Node({value, nullptr, nullptr});
    start->next = start;
    start->prev = start;
}

// copy constructor that creates a copy of the parameter DataLoop (rhs)
DataLoop::DataLoop(const DataLoop & rhs) {
    start = nullptr;
    count = 0;

    // uses assignment operator to update elements of new DataLoop
    *this = rhs;
}

// assignment operator that assigns a DataLoop to another DataLoop
DataLoop & DataLoop::operator=(const DataLoop & rhs) {

    // deallocates dynamically allocated memory in the implicit DataLoop parameter
    clear();

    _Node *cur_node = rhs.start;

    // updates contents of implicit DataLoop parameter
    for (size_t i = 0; i < rhs.count; i++) {
        *this += cur_node->data; // count is updated by += function
        cur_node = cur_node->next;
    }

    return *this;
}

// deallocates dynamically allocated memory in DataLoop
void DataLoop::clear() {
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
DataLoop::~DataLoop() {
    clear(); 
}

// compares the current DataLoop with the input DataLoop, returning true if they're the same node by node
bool DataLoop::operator==(const DataLoop & rhs) const {

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

// adds a value to the end of the DataLoop
DataLoop & DataLoop::operator+=(const int & num) {

    // new node to be added to dataloop
    _Node *new_node = new _Node({num, nullptr, nullptr});

    // adds the new node to an empty dataloop
    if (count == 0 && start == nullptr) {
        start = new_node;
        new_node->prev = start;
        new_node->next = start;
        count++;
    }
    else {
        // sets cur_node to last node in the dataloop
        _Node *cur_node = start;
        while (cur_node->next != start) {
            cur_node = cur_node->next;
        }

        // adds new node to the dataloop
        new_node->prev = cur_node;
        new_node->next = start;

        // updates pointers to adjacent nodes of second-to-last node and first node
        cur_node->next = new_node;
        start->prev = new_node;
        count++;
    }

    return *this;
}

// creates a third DataLoop by concatenating copies of the current DataLoop and the new DataLoop
DataLoop DataLoop::operator+(const DataLoop & rhs) const {

    DataLoop new_data_loop = *this; // = operator

    // rhs DataLoop contains no nodes
    if (rhs.count == 0 && rhs.start == nullptr) {
        return new_data_loop;
    }
    // current DataLoop contains no nodes
    else if (count == 0 && start == nullptr) {
        new_data_loop = rhs; // = operator
        return new_data_loop;
    }

    _Node *cur_node = rhs.start;

    for (size_t i = 0; i < rhs.count; i++) {
        new_data_loop += cur_node->data; // += operator updates count
        cur_node = cur_node->next;
    }

    return new_data_loop;
}

// shifts the start position in *this DataLoop according to the parameter offset
// forward for a positive value and backward for a negative value
DataLoop & DataLoop::operator^(int offset) {

    // no change made to start if DataLoop is empty, DataLoop has one node, or the offset is 0
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

// inserts the entire parameter DataLoop (rhs) into the current DataLoop (*this)
// at the indicated position (pos) and makes rhs an empty list
DataLoop & DataLoop::splice(DataLoop & rhs, size_t pos) {

    // rhs has no nodes
    if (rhs.count == 0) {
        rhs.clear();
        return *this;
    }
    // current DataLoop has no nodes
    else if (count == 0) {
        *this = rhs; // uses overloaded assignment operator
        rhs.clear();
        return *this;
    }

    // inserts rhs DataLoop into current DataLoop and updates starts
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
    // inserts rhs DataLoop into current DataLoop and does not update start
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

// outputs the value of each node in the DataLoop
std::ostream & operator<<(std::ostream & os, const DataLoop & dl) {
    if (dl.count == 0) {
        os << ">no values<";
    }
    else {
        DataLoop::_Node *cur_node = dl.start;
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


