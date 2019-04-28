#ifndef DOUBLELIST_H
#define DOUBLELIST_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

template<typename T>
class listItem {
public:
  listItem(T i) :
    prev(nullptr), next(nullptr) {
    payload = (T)malloc(sizeof(i));
    memcpy(payload, i, sizeof(*i));
  }
  listItem<T>* prev;
  listItem<T>* next;
  T payload;
};

template<typename T>
class doubleList {
private:
//  template<T>
  listItem<T>* head;
//  template<T>
  listItem<T>* tail;
  int length;
public:
  doubleList() {
    head = nullptr;
    tail = nullptr;
    length = 0;
  }
  ~doubleList() {
    if (!isEmpty()) {
      empty();
    }
  }

  void empty() {
    T tmp = nullptr;
    while (head != nullptr) {
      tmp = pop();
      delete(tmp);
    }
  };

  listItem<T>* begin() { return head; }
  listItem<T>* end() { return nullptr;}
  listItem<T>* rbegin() { return tail; }

  void push(T item) {
    listItem<T>* itm = new listItem<T>(item);
    itm->next = head;
    itm->prev = nullptr;
    if (head == nullptr) {
      tail = itm;
    }
    head = itm;
    ++length;
  }
  
  void append(T item) {
    listItem<T>* itm = new listItem<T>(item);
    itm->next = nullptr;
    if (head == nullptr) {
      head = itm;
    } else {
      tail->next = itm;
      itm->prev = tail;
    }
    tail = itm;
    ++length;
  }

  T pop() {
    if (head == nullptr) {
      return nullptr;
    }
    listItem<T>* ret = head;
    head->prev = nullptr;
    head = ret->next;
    --length;
    T r = ret->payload;
    delete(ret);
    return r;
  }

  bool isEmpty() {
    return head == nullptr;
  }
  
  int getLength() {
    return length;
  }

  T find(int key) {
    if (length <= key) {
      return nullptr;
    }
  
    listItem<T>* ret = head;
    for(int i = 0; i < key; ++i) {
      ret = ret->next;
    }
    return ret->payload;
  }
  
  T remove(int key) {
    if (length <= key) {
      return nullptr;
    }
    listItem<T>* ret = head;
    for(int i = 0; i < key; ++i) {
      ret = ret->next;
    }
    T r = ret->payload;
    ret->prev->next = ret->next;
    ret->next->prev = ret->prev;
    delete(ret);
    return r;
  }
};

#endif
