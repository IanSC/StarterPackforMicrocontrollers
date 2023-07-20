#pragma once

// minimal C++ vector
// original Arduino has no C++ library
// and some has limited space for full library

namespace StarterPack {

template<class T>
class spVector {

//
// DATA
//
protected:

    struct data {
        T* payload;
        data* next = nullptr;
    };

    data *head = nullptr;

//
// INIT
//
public:

    // if payload will also be deleted
    // otherwise up to caller to delete them
    bool deletePayload = true;

    ~spVector() {
        data *parent = head;
        while( parent != nullptr ) {
            data *child = parent->next;
            if (deletePayload)
                delete parent->payload;
            delete parent;
            parent = child;
        }
    }

//
// INSERT
//
public:

    unsigned int count = 0;

    void insert(T *payload) {
        auto newEntry = new data;
        newEntry->payload = payload;

        if ( head == nullptr )
            head = newEntry;
        else {
            // add to end of list
            data *p = head;
            while ( p->next != nullptr )
                p = p->next;
            p->next = newEntry;
        }
        count++;
    }

//
// ITERATE
//
public:

    data *iter = nullptr;

    T* getFirst() {
        iter = head;
        return getCurrent();
    }

    T* getCurrent() {
        if (iter == nullptr) return nullptr;
        return iter->payload;
    }

    T* getNext() {
        if (iter != nullptr)
            iter = iter->next;
        return getCurrent();
    }

};

}
