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
        clear();
    }

    void clear() {
        data *parent = head;
        while( parent != nullptr ) {
            data *child = parent->next;
            if (deletePayload)
                delete parent->payload;
            delete parent;
            parent = child;
        }
        parent = nullptr;
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

// BEFORE VECTOR, IMPLEMENTED AS LINKED-LIST

//
// NEXT-LINKED LIST
//

    /*
    
    USE: spVector<>

    // insert new item as head or at end of list
    // A* head = nullptr;
    // class A {
    //    ...
    //    A* next;
    // }
    template<typename T>
    void insertEnd_NextLinkedList( T **head, T *item ) {
        if ( *head == nullptr )
            *head = item;
        else {
            T *p = *head;
            while ( p->next != nullptr ) p = p->next;
            p->next = item;
        }
    }

    // delete "next-linked" list
    // class A {
    //    ...
    //    A* next;
    // }
    template<class T>
    void delete_NextLinkedList( T *head ) {
        if ( head == nullptr ) return;
        while( head->next != nullptr ) {
            // slow but avoids recursion
            T *p1 = head;
            T *p2 = p1->next;
            while ( p2->next != nullptr ) {
                p1 = p2;
                p2 = p2->next;
            }
            delete p2;
            p1->next = nullptr;
        }
        delete head;
    }

    // delete "next-linked" list
    // class A {
    //    ...
    //    bool skipDelete;
    //    A* next;
    // }
    template<class T>
    void delete_NextLinkedListWithSkip( T *head ) {
        if ( head == nullptr ) return;
        while( head->next != nullptr ) {
            // slow but avoids recursion
            T *p1 = head;
            T *p2 = p1->next;
            while ( p2->next != nullptr ) {
                p1 = p2;
                p2 = p2->next;
            }
            if ( !p2->skipDelete )
                delete p2;
            p1->next = nullptr;
        }
        delete head;
    }
    */
