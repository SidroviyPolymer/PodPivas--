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




template <typename T>
Elem<T>::Elem() {
	data = nullptr;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
Elem<T>::Elem(T& data) {
	this->data = data;
	prev = nullptr;
	next = nullptr;
}

template <typename T>
Elem<T>::Elem(T& data, Elem* prev, Elem* next) {
	this->data = data;
	this->prev = prev;
	this->next = next;
}