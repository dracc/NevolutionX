#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <stdlib.h>

template<class T>
class vector {
private:
  T** items;
  size_t m_length;
  size_t m_size;
public:

  typedef T* iterator;

  vector() {
    m_size = 4;
    items = static_cast<T**>(malloc(m_size * sizeof(T*)));
    m_length = 0;
  }

  ~vector() {
    if (!empty()) {
      clear();
    }
    free(items);
  }

  void clear() {
    for (int i = 0; i < m_length; ++i) {
      items[i]->~T();
      free(items[i]);
      items[i] = nullptr;
    }
    for (int i = m_length; i < m_size; ++i) {
      free(items[i]);
      items[i] = nullptr;
    }
    m_length = 0;
  };

  void push_back(const T & item) {
    if (m_length == m_size) {
      m_size = m_size<<1;
      items = static_cast<T**>(realloc(items, m_size * sizeof(T*)));
    }
    T* t = new T(item);
    items[m_length] = t;
    ++m_length;
  }

  void pop_back() {
    items[m_length - 1]->~T;
    free(items[m_length - 1]);
    items[m_length - 1] = nullptr;
    --m_length;
  }

  bool empty() const{
    return m_length == 0;
  }

  size_t size() const{
    return m_length;
  }

  size_t capacity() const{
    return m_size;
  }

  T& at(size_t key) {
    if (m_length <= key) {
#ifndef NXDK
      throw "Out of bounds";
#else
      // We should probably do something here but `throw`
      // is not yet implemented in NXDK.
#endif
    }
    return *items[key];
  }

  T& operator[](size_t key) {
    return *items[key];
  }

  iterator begin() {
    return items;
  }

  iterator end() {
    return items + m_length;
  }

  T& front() {
    return *items[0];
  }

  T& back() {
    return *items[m_length - 1];
  }

  T erase(size_t key) {
    if (m_length <= key) {
      return nullptr;
    }
    T ret = *items[key];
    for (size_t i = key; i < m_length - 1; ++i) {
      items[i] = items[i + 1];
    }
    items[m_length] = nullptr;
    --m_length;
    return ret;
  }
};

#endif
