#pragma once

template <typename T>
class List {
private:
    class Elem {
    public:
        T data;
        Elem* prev;
        Elem* next;


        Elem();
        Elem(T& data);
        Elem(T& data, Elem* prev, Elem* next);
    };

    Elem* begin;
    Elem* end;
    size_t size;

    template <typename... Args>
    void CreateList(T& data, Args... args);

    void CreateList(T& data);

    void CreateList();

    Elem* FindByIdx(size_t idx);
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
};

