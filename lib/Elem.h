#pragma once

template <typename T>
class Elem {
public:
    T data;
    Elem* prev;
    Elem* next;


    Elem();
    Elem(T& data);
    Elem(T& data, Elem* prev, Elem* next);
};