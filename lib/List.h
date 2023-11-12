#pragma once
#include "Elem.h"

template <typename T>
class List {
private:
    Elem<T>* begin;
    Elem<T>* end;
    size_t size;

    template <typename... Args>
    void CreateList(T& data, Args... args);

    void CreateList(T& data);

    void CreateList();

    Elem<T>* FindByIdx(size_t idx);
public:

    List();

    template <typename... Args>
    List(Args... args);

    ~List();

    void Push_front(T& data);

    void Push_back(T& data);

    void Push_at(size_t idx, T& data);

    T& Pop_front();

    T& Pop_back();

    T& Pop_at(size_t idx);

    T& At(size_t idx);

    size_t Length();
};

